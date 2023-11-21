/**********************************************************/
/* Mini web server por Dario Alpern (17/8/2020)           */
/*                                                        */
/* Headers HTTP relevantes del cliente:                   */
/* GET {path} HTTP/1.x                                    */
/*                                                        */
/* Encabezados HTTP del servidor enviados como respuesta: */
/* HTTP/1.1 200 OK                                        */
/* Content-Length: nn (longitud del HTML)                 */
/* Content-Type: text/html; charset=utf-8                 */
/* Connection: Closed                                     */
/*                                                        */
/* Después de los encabezados va una línea en blanco y    */
/* luego el código HTML.                                  */
/*                                                        */
/* http://dominio/tempCelsius (número): genera respuesta  */
/* con temperatura en Celsius indicado por el usuario y   */
/* en Fahrenheit.                                         */
/**********************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <errno.h>
#define MAX_CONN 10 //Nro maximo de conexiones en espera

void ProcesarCliente(int s_aux, struct sockaddr_in *pDireccionCliente,
                     int puerto);

/*---------------Global Variables Begin---------------*/
config_t *configuracionServer = (void *) 0;
sensor_t *SensorData = (void *) 0;
void *sharMemSensor = (void *)0;
void *sharMemConfig = (void *)0;
int semaforoSensor, semaforoConfig;

/*---------------Global Variables End---------------*/

/**********************************************************/
/* funcion MAIN                                           */
/* Orden Parametros: Puerto                               */
/**********************************************************/
int main(int argc, char *argv[])
{
  int s;
  int r;
  int sharMemIdSensor,sharMemIdConfig;
  struct sockaddr_in datosServidor;
  socklen_t longDirec;

  r = Init_Process (&sharMemIdSensor, &sharMemIdConfig);
  if (r == -1)
    return -1;
  printf("|*******************************************************\n");
  printf("|Configuracion de Procesos Realizada                    \n");
  printf("|*******************************************************\n");


  r = Init_Server 


  if (argc != 2)
  {
    printf("\n\nLinea de comandos: webserver Puerto\n\n");
    exit(1);
  }
  // Creamos el socket
  s = socket(AF_INET, SOCK_STREAM,0);
  if (s == -1)
  {
    printf("ERROR: El socket no se ha creado correctamente!\n");
    exit(1);
  }
  // Asigna el puerto indicado y una IP de la maquina
  datosServidor.sin_family = AF_INET; //Tipo de dirrecion. En este caso IPv4 
  datosServidor.sin_port = htons(atoi(argv[1])); //Carga el puerto
  datosServidor.sin_addr.s_addr = htonl(INADDR_ANY); //Cargo la IP 0.0.0.0 que sera la comun

  // Obtiene el puerto para este proceso.
  if( bind(s, (struct sockaddr*)&datosServidor,
           sizeof(datosServidor)) == -1)
  {
    printf("ERROR: este proceso no puede tomar el puerto %s\n",
           argv[1]);
    exit(1);
  }
  printf("\nIngrese en el navegador http://dir ip servidor:%s/gradosCelsius\n",
         argv[1]);
  // Indicar que el socket encole hasta MAX_CONN pedidos
  // de conexion simultaneas.
  if (listen(s, MAX_CONN) < 0)
  {
    perror("Error en listen");
    close(s);
    exit(1);
  }
  // Permite atender a multiples usuarios
  while (1)
  {
    int pid, s_aux;
    struct sockaddr_in datosCliente;
    // La funcion accept rellena la estructura address con
    // informacion del cliente y pone en longDirec la longitud
    // de la estructura.
    longDirec = sizeof(datosCliente);
    s_aux = accept(s, (struct sockaddr*) &datosCliente, &longDirec);
    if (s_aux < 0)
    {
      perror("Error en accept");
      close(s);
      exit(1);
    }
    pid = fork();
    if (pid < 0)
    {
      perror("No se puede crear un nuevo proceso mediante fork");
      close(s);
      exit(1);
    }
    if (pid == 0)
    {       // Proceso hijo.
      ProcesarCliente(s_aux, &datosCliente, atoi(argv[1]));
      exit(0);
    }
    close(s_aux);  // El proceso padre debe cerrar el socket
                   // que usa el hijo.
  }
}

void ProcesarCliente(int s_aux, struct sockaddr_in *pDireccionCliente,
                     int puerto)
{
  char bufferComunic[4096];
  char ipAddr[20];
  int Port;
  int indiceEntrada;
  float tempCelsius;
  int tempValida = 0;
  char HTML[4096];
  char encabezadoHTML[4096];
  
  strcpy(ipAddr, inet_ntoa(pDireccionCliente->sin_addr));
  Port = ntohs(pDireccionCliente->sin_port);
  // Recibe el mensaje del cliente
  if (recv(s_aux, bufferComunic, sizeof(bufferComunic), 0) == -1)
  {
    perror("Error en recv");
    exit(1);
  }
  printf("* Recibido del navegador Web %s:%d:\n%s\n",
         ipAddr, Port, bufferComunic);
  
  // Obtener la temperatura desde la ruta.
  if (memcmp(bufferComunic, "GET /", 5) == 0)
  {
    if (sscanf(&bufferComunic[5], "%f", &tempCelsius) == 1)
    {      // Conversion done successfully.
      tempValida = 1;
    }
  }
  
  // Generar HTML.
  // El viewport es obligatorio para que se vea bien en
  // dispositivos móviles.
  sprintf(encabezadoHTML, "<html><head><title>Temperatura</title>"
            "<meta name=\"viewport\" "
            "content=\"width=device-width, initial-scale=1.0\">"
            "</head>"
            "<h1>Temperatura</h1>");
  if (tempValida)
  {
    sprintf(HTML, 
            "%s<p>%f grados Celsius equivale a %f grados Fahrenheit</p>",
            encabezadoHTML, tempCelsius, tempCelsius * 1.8 + 32);
  }
  else
  {
    sprintf(HTML, 
            "%s<p>El URL debe ser http://dominio:%d/gradosCelsius.</p>",
            encabezadoHTML, puerto);
  }

  sprintf(bufferComunic,
          "HTTP/1.1 200 OK\n"
          "Content-Length: %ld\n"
          "Content-Type: text/html; charset=utf-8\n"
          "Connection: Closed\n\n%s",
          strlen(HTML), HTML);

  printf("* Enviado al navegador Web %s:%d:\n%s\n",
         ipAddr, Port, bufferComunic);
  
  // Envia el mensaje al cliente
  if (send(s_aux, bufferComunic, strlen(bufferComunic), 0) == -1)
  {
    perror("Error en send");
    exit(1);
  }
  // Cierra la conexion con el cliente actual
  close(s_aux);
}


int Init_Process (int *sharMemIdSensor, int *sharMemIdConfig){

/* SHARED MEMORY*/
  //sensor
  sharMemSensor = (sensor_t *)creo_SharedMemory(sharMemIdSensor, KEY_SENSOR);
  if (SensorData == (void *)-1)
  {
      printf("|ERROR: No se pudo obtener Shared Memory para el sensor\n");
      return -1;
  }
  //configuracion
  sharMemConfig = (config_t *)creo_SharedMemory(sharMemIdConfig, KEY_CONFIG);
  if (configuracionServer == (void *)-1)
  {
      printf("|ERROR: No se pudo obtener Shared Memory para la Configuracion\n");
      return -1;
  }
    SensorData = (sensor_t *)sharMemSensor;
    configuracionServer = (config_t *)sharMemConfig;


/* SEMAFORO */
    //Sensor
    if (crear_semaforo(&semaforoSensor, KEY_SENSOR) < 0)
    {
        perror("|ERROR: No se pudo crear el semaforo para el sensor\n");
        semctl(semaforoSensor, 0, IPC_RMID);     //Cierro el semaforo del sensor
        shmdt(sharMemSensor);                    //Separo la memoria del sensor del proceso
        shmdt(sharMemConfig);                    //Separo la memoria de la configuracion del proceso
        shmctl(sharMemIdSensor, IPC_RMID, NULL); //Cierro la Shared Memory del sensor
        shmctl(sharMemIdConfig, IPC_RMID, NULL); //Cierro la Shared Memory de la configuracion
        return -1;
    }

    //Creacion del semaforo para la configuracion
    if (crear_semaforo(&semaforoConfig, KEY_CONFIG) < 0)
    {
        perror("|ERROR: No se pudo crear el semaforo para la configuracion\n");
        semctl(semaforoSensor, 0, IPC_RMID);     //Cierro el semaforo del sensor
        semctl(semaforoConfig, 0, IPC_RMID);     //Cierro el semaforo de la configuracion
        shmdt(sharMemSensor);                    //Separo la memoria del sensor del proceso
        shmdt(sharMemConfig);                    //Separo la memoria de la configuracion del proceso
        shmctl(sharMemIdSensor, IPC_RMID, NULL); //Cierro la Shared Memory del sensor
        shmctl(sharMemIdConfig, IPC_RMID, NULL); //Cierro la Shared Memory de la configuracion
        return -1;
    }
}


/*
* Creacion  y configuracion del Servidor
*/
int Init_Server (int *sock,){

//--------Configuracion del Server----------
  semop(semaforoConfig, &tomar, 1); //Tomo el semaforo
  configuracion_init(configuracionServer);
  semop(semaforoConfig, &liberar, 1); //Libreo el semaforo


  sock = socket(AF_INET, SOCK_STREAM, 0);
  if (sock == -1)
  {
      printf("|ERROR: El socket no se ha creado correctamente!\n");
      semctl(semaforoSensor, 0, IPC_RMID);     //Cierro el semaforo del sensor
      semctl(semaforoConfig, 0, IPC_RMID);     //Cierro el semaforo de la configuracion
      shmdt(sharMemSensor);                    //Separo la memoria del sensor del proceso
      shmdt(sharMemConfig);                    //Separo la memoria de la configuracion del proceso
      shmctl(sharMemIdSensor, IPC_RMID, NULL); //Cierro la Shared Memory del sensor
      shmctl(sharMemIdConfig, IPC_RMID, NULL); //Cierro la Shared Memory de la configuracion
      exit(1);
  }

  // Asigna el puerto indicado y la IP de la maquina
  datosServidor.sin_family = AF_INET;
  datosServidor.sin_port = htons(atoi(argv[1]));
  datosServidor.sin_addr.s_addr = htonl(INADDR_ANY);

  // Obtiene el puerto para este proceso.
  if (bind(sock, (struct sockaddr *)&datosServidor, sizeof(datosServidor)) == -1)
  {
      printf("|ERROR: este proceso no puede tomar el puerto %s\n", argv[1]);
      semctl(semaforoSensor, 0, IPC_RMID);     //Cierro el semaforo del sensor
      semctl(semaforoConfig, 0, IPC_RMID);     //Cierro el semaforo de la configuracion
      shmdt(sharMemSensor);                    //Separo la memoria del sensor del proceso
      shmdt(sharMemConfig);                    //Separo la memoria de la configuracion del proceso
      shmctl(sharMemIdSensor, IPC_RMID, NULL); //Cierro la Shared Memory del sensor
      shmctl(sharMemIdConfig, IPC_RMID, NULL); //Cierro la Shared Memory de la configuracion
      close(sock);
      exit(1);
  }

  printf("|Ingrese en el navegador http://192.168.1.XX:%s\n", argv[1]);

  // Indicar que el socket encole hasta backlog pedidos de conexion simultaneas.
  semop(semaforoConfig, &tomar, 1); //Tomo el semaforo
  auxBacklog = configuracionServer->backlog;
  semop(semaforoConfig, &liberar, 1); //Libreo el semaforo
  if (listen(sock, auxBacklog) < 0)
  {
      perror("|Error en listen");
      shmdt(sharMemSensor);                    //Separo la memoria del proceso
      semctl(semaforoSensor, 0, IPC_RMID);     //Cierro el semaforo
      shmctl(sharMemIdSensor, IPC_RMID, NULL); //Cierro la Shared Memory
      close(sock);
      exit(1);
  }
}