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
#include "../inc/Servidor.h"


#define MAX_CONN 10 //Nro maximo de conexiones en espera


/*---------------Global Variables Begin---------------*/
config_t *configuracionServer = (void *) 0;
sensor_t *SensorData = (void *) 0;
void *sharMemSensor = (void *)0;
void *sharMemConfig = (void *)0;
int semaforoSensor, semaforoConfig;
struct sembuf tomar = {0, -1, SEM_UNDO};   // Estructura para tomar el semáforo
struct sembuf liberar = {0, +1, SEM_UNDO}; // Estructura para liberar el semáforo
struct sockaddr_in datosServidor;
int sharMemIdSensor,sharMemIdConfig;
struct timeval timeout;
int flag_exit = 0;
int flag_config = 0;
/*---------------Global Variables End---------------*/

/**********************************************************/
/* funcion MAIN                                           */
/* Orden Parametros: Puerto                               */
/**********************************************************/
int main(int argc, char *argv[])
{
  int sock;
  int res;
  int sensorPID, configPID;
  int arg_port;
  int auxConexMAX, auxConexiones;
  socklen_t longDirec;

  arg_port = atoi(argv[1]);
  res = Init_IPC (&sharMemIdSensor, &sharMemIdConfig);
  if (res == -1)
    return -1;
  printf("|*******************************************************\n");
  printf("|Configuracion de Procesos Realizada                    \n");
  printf("|*******************************************************\n");

  signal(SIGUSR2, SIGUSR2_handler);
  signal(SIGCHLD, SIGCHLD_handler);
  signal(SIGINT, SIGINT_handler);
  


  res = Init_Server(&sock,arg_port);
  if (res == -1)
    return -1;
  printf("|*******************************************************\n");
  printf("|Configuracion de Servidor Realizada                    \n");
  printf("|*******************************************************\n");


/* ---- Control del Sensor ----*/
  sensorPID = fork();
  if (sensorPID < 0)
  {
      semctl(semaforoSensor, 0, IPC_RMID);     //Cierro el semaforo del sensor
      shmdt(sharMemSensor);                    //Separo la memoria del sensor del proceso
      shmctl(sharMemIdSensor, IPC_RMID, NULL); //Cierro la Shared Memory del sensor
      close(sock);
      exit(1);
  }
  if (sensorPID == 0)
  {
      //Funcion para manejar el sensor
      //signal(SIGINT, SIGINT_handler_Sensor);
      Sensor_readData();

      semctl(semaforoSensor, 0, IPC_RMID);     //Cierro el semaforo del sensor
      shmdt(sharMemSensor);                    //Separo la memoria del sensor del proceso
      shmctl(sharMemIdSensor, IPC_RMID, NULL); //Cierro la Shared Memory del sensor
      close(sock);
      exit(1);
  }

    configPID = fork();
    if (configPID < 0)
    {
        semctl(semaforoConfig, 0, IPC_RMID);     //Cierro el semaforo de la configuracion
        shmdt(sharMemConfig);                    //Separo la memoria de la configuracion del proceso
        shmctl(sharMemIdConfig, IPC_RMID, NULL); //Cierro la Shared Memory de la configuracion
        close(sock);
        exit(1);
    }
    if (configPID == 0)
    {
        //Funcion para manejar la configuracion
        ManejadorConfiguracion();

        semctl(semaforoConfig, 0, IPC_RMID);     //Cierro el semaforo de la configuracion
        shmdt(sharMemConfig);                    //Separo la memoria de la configuracion del proceso
        shmctl(sharMemIdConfig, IPC_RMID, NULL); //Cierro la Shared Memory de la configuracion
        close(sock);
        exit(1);
    }
printf("|Proceso Config ID = %d\n", configPID);
 
/* ---- Atender usuarios ----*/
  while (!flag_exit){
    int pid, sock_aux;
    struct sockaddr_in datosCliente;
    fd_set readfds;
    int nbr_fds;

/*Lectura de la configuracion y conexiones maximas*/
    semop(semaforoConfig, &tomar, 1);                  //Tomo el semaforo
    auxConexMAX = configuracionServer->conexiones_max; //Cantidad de conexiomes permitidas
    auxConexiones = configuracionServer->conexiones;   //conexiones actuales
    semop(semaforoConfig, &liberar, 1);                //Libreo el semaforo
  if (auxConexiones < auxConexMAX){
    // La funcion accept rellena la estructura address con
    // informacion del cliente y pone en longDirec la longitud
    // de la estructura.
    FD_ZERO(&readfds);

    // Especificamos el socket, podria haber mas.
    FD_SET(sock, &readfds);

    timeout.tv_sec = 1;
    timeout.tv_usec = 0;

    //Si no hay clientes, el servidor no debe ser bloqueado por accept().
    //Con select () un temporizador espera nuevos datos en el socket,
    //si no hay datos en el socket el servidor se salta de accept ().

    nbr_fds = select(sock + 1, &readfds, NULL, NULL, &timeout);
    if ((nbr_fds < 0) && (errno != EINTR))
    {
        perror("|select");
    }
    if (!FD_ISSET(sock, &readfds))
    {
        continue;
    }
    if (nbr_fds > 0){
      longDirec = sizeof(datosCliente);
      sock_aux = accept(sock, (struct sockaddr*) &datosCliente, &longDirec);
      if (sock_aux < 0)
      {
        perror("Error en accept");
        End_IPC();
        close(sock);
        exit(1);
      }
      printf("|Nueva Conexion Entrante                   \n");
      pid = fork();
      if (pid < 0)
      {
        perror("No se puede crear un nuevo proceso mediante fork");
        End_IPC();
        close(sock);
        exit(1);
      }
      if (pid == 0){       // Proceso hijo que atienda al cliente
        semop(semaforoConfig, &tomar, 1); //Tomo el semaforo
        configuracionServer->conexiones++;
        semop(semaforoConfig, &liberar, 1); //Libreo el semaforo
        ProcesarCliente(sock_aux, &datosCliente, atoi(argv[1]));
        semop(semaforoConfig, &tomar, 1); //Tomo el semaforo
        configuracionServer->conexiones--;
        semop(semaforoConfig, &liberar, 1); //Libreo el semaforo
        exit(0);
      }
      close(sock_aux);  // El proceso padre debe cerrar el socket
                    // que usa el hijo.
    }
  }
  }
      while(1) 
    {
        printf("|----------------------------------------------------\n");
        printf("|Terminando Servidor...\n");
        printf("|----------------------------------------------------\n");
        sleep(5);
        sleep(5);
        close(sock);
        semctl(semaforoSensor, 0, IPC_RMID);     //Cierro el semaforo del sensor
        shmdt(sharMemSensor);                    //Separo la memoria del sensor del proceso
        shmctl(sharMemIdSensor, IPC_RMID, NULL); //Cierro la Shared Memory del sensor
        semctl(semaforoConfig, 0, IPC_RMID);     //Cierro el semaforo de la configuracion
        shmdt(sharMemConfig);                    //Separo la memoria de la configuracion del proceso
        shmctl(sharMemIdConfig, IPC_RMID, NULL); //Cierro la Shared Memory de la configuracion
        
        printf("|----------------------------------------------------\n");
        printf("|Servidor Finalizadon...\n");
        printf("|----------------------------------------------------\n");
        return 0;
    
        sleep(1);
    }
    return 0;
}

void ProcesarCliente(int s_aux, struct sockaddr_in *pDireccionCliente,
                     int puerto)
{
  char bufferComunic[4096];
  char ipAddr[20];
  int Port;
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
  
  
  // Generar HTML.
  // El viewport es obligatorio para que se vea bien en
  // dispositivos móviles.
  semop(semaforoSensor, &tomar, 1); //Tomo el semaforo
  sprintf(encabezadoHTML, "<html><head><title>Temperatura</title>"
            "<meta name=\"viewport\" "
            "content=\"width=device-width, initial-scale=1.0\">"
            "</head>"
            "<h1>Temperatura</h1>");
 
  sprintf(HTML,
    "%s"
    "<head>"
    "   <style>"
    "       body { background-color: #f2f2f2; font-family: Arial, sans-serif; }"
    "       h2 { color: #333; }"
    "       h3 { color: #555; }"
    "       table { width: 50%; border-collapse: collapse; margin-top: 20px; }"
    "       th, td { padding: 10px; text-align: left; border: 1px solid #ddd; }"
    "       th { background-color: #4CAF50; color: white; }"
    "   </style>"
    "</head>"
    "<body>"
    "<h2>Servidor Web</h2>"
    "<h3>Valores del sensor MPU6050:</h3>"
    "<table>"
    "<tr><th>Parámetro</th><th>Valor</th></tr>"
    "<tr><td><strong>accel_Xout</strong></td><td>%.02fg</td></tr>"
    "<tr><td><strong>accel_Yout</strong></td><td>%.02fg</td></tr>"
    "<tr><td><strong>accel_Zout</strong></td><td>%.02fg</td></tr>"
    "<tr><td><strong>temp_out</strong></td><td>%.02f°</td></tr>"
    "<tr><td><strong>gyro_Xout</strong></td><td>%.02f°/s</td></tr>"
    "<tr><td><strong>gyro_Yout</strong></td><td>%.02f°/s</td></tr>"
    "<tr><td><strong>gyro_Zout</strong></td><td>%.02f°/s</td></tr>"
    "</table>"
    "</body>",
    encabezadoHTML, 
    SensorData->accel_xout, SensorData->accel_yout, SensorData->accel_zout,
    SensorData->temp_out, SensorData->gyro_xout, SensorData->gyro_yout, SensorData->gyro_zout);

  sprintf(bufferComunic,
          "HTTP/1.1 200 OK\n"
          "Content-Length: %ld\n"
          "Content-Type: text/html; charset=utf-8\n"
          "Connection: Closed\n\n%s",
          strlen(HTML), HTML);
  semop(semaforoSensor, &liberar, 1); //Libreo el semaforo

  printf("* Enviado al navegador Web %s:%d:\n%s\n",
         ipAddr,   Port, bufferComunic);
  
  // Envia el mensaje al cliente
  if (send(s_aux, bufferComunic, strlen(bufferComunic), 0) == -1)
  {
    perror("Error en send");
    exit(1);
  }
  // Cierra la conexion con el cliente actual
  close(s_aux);
}


int Init_IPC (int *sharMemIdSensor, int *sharMemIdConfig){

/* SHARED MEMORY*/
  //sensor
  sharMemSensor = (sensor_t *)shmem_create(sharMemIdSensor, KEY_SENSOR);
  if (SensorData == (void *)-1)
  {
      printf("|ERROR: No se pudo obtener Shared Memory para el sensor\n");
      return -1;
  }
  //configuracion
  sharMemConfig = (config_t *)shmem_create(sharMemIdConfig, KEY_CONFIG);
  if (configuracionServer == (void *)-1)
  {
      printf("|ERROR: No se pudo obtener Shared Memory para la Configuracion\n");
      return -1;
  }
    SensorData = (sensor_t *)sharMemSensor;
    configuracionServer = (config_t *)sharMemConfig;


/* SEMAFORO */
    //Sensor
    if (sem_create(&semaforoSensor, KEY_SENSOR) < 0)
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
    if (sem_create(&semaforoConfig, KEY_CONFIG) < 0)
    {
      perror("|ERROR: No se pudo crear el semaforo para la configuracion\n");
      End_IPC ();
      return -1;
    }
}


/*
* Creacion  y configuracion del Servidor
*/
int Init_Server (int *sock, int arg_port){
  int auxBacklog;

//--------Configuracion del Server----------
  semop(semaforoConfig, &tomar, 1); //Tomo el semaforo
  configuracion_init(configuracionServer);
  semop(semaforoConfig, &liberar, 1); //Libreo el semaforo


  *sock = socket(AF_INET, SOCK_STREAM, 0);
  if (*sock == -1)
  {
      printf("|ERROR: El socket no se ha creado correctamente!\n");
      End_IPC ();
      exit(1);
  }

  // Asigna el puerto indicado y la IP de la maquina
  datosServidor.sin_family = AF_INET;
  datosServidor.sin_port = htons(arg_port);
  datosServidor.sin_addr.s_addr = htonl(INADDR_ANY);

  // Obtiene el puerto para este proceso.
  if (bind(*sock, (struct sockaddr *)&datosServidor, sizeof(datosServidor)) == -1)
  {
      printf("|ERROR: este proceso no puede tomar el puerto %d\n", arg_port);
      End_IPC ();
      close(*sock);
      exit(1);
  }


  // Indicar que el socket encole hasta backlog pedidos de conexion simultaneas.
  semop(semaforoConfig, &tomar, 1); //Tomo el semaforo
  auxBacklog = configuracionServer->backlog;
  semop(semaforoConfig, &liberar, 1); //Libreo el semaforo
  if (listen(*sock, auxBacklog) < 0)
  {
      perror("|Error en listen");
      End_IPC ();
      close(sock);
      exit(1);
  }
  return 1;
}


void End_IPC (){
  semctl(semaforoSensor, 0, IPC_RMID);     //Cierro el semaforo del sensor
  shmdt(sharMemSensor);                    //Separo la memoria del sensor del proceso
  shmctl(sharMemIdSensor, IPC_RMID, NULL); //Cierro la Shared Memory del sensor
  semctl(semaforoConfig, 0, IPC_RMID);     //Cierro el semaforo de la configuracion
  shmdt(sharMemConfig);                    //Separo la memoria de la configuracion del proceso
  shmctl(sharMemIdSensor, IPC_RMID, NULL);
}



void configuracion_init(config_t *serverConf)
{
    t_config *config;

    //Leo el archivo de configuracion
    config = config_create(CONFIG_RUTA);
    if (config != NULL)
    {
        //El archivo existe, lo leo.
        if (config_has_property(config, "CONEXIONES_MAX") == true)
        {
            serverConf->conexiones_max = config_get_int_value(config, "CONEXIONES_MAX");
        }
        else
        {
            serverConf->conexiones_max = CANT_CONEX_MAX_DEFAULT;
        }
        if (config_has_property(config, "BACKLOG") == true)
        {
            serverConf->backlog = config_get_int_value(config, "BACKLOG");
        }
        else
        {
            serverConf->backlog = BACKLOG_DEFAULT;
        }
        if (config_has_property(config, "MUESTREO_SENSOR") == true)
        {
            serverConf->muestreo = config_get_int_value(config, "MUESTREO_SENSOR");
        }
        else
        {
            serverConf->muestreo = MUESTREO_SENSOR;
        }
    }
    else
    {
        //No existe el archivo de configuracion, utilizo valores por default
        printf("No existe el archivo de configuracion\n");
        serverConf->backlog = BACKLOG_DEFAULT;
        serverConf->conexiones_max = CANT_CONEX_MAX_DEFAULT;
        serverConf->muestreo = MUESTREO_SENSOR;
    }

    serverConf->conexiones = 0;
}


void ManejadorConfiguracion(void)
{
    printf("Manejador de la configuracion\n");

    while (!flag_exit)
    {
        if (flag_config == true)
        {
            //--------Configuracion del Server----------
            semop(semaforoConfig, &tomar, 1); //Tomo el semaforo
            configuracion_reload(configuracionServer);
            semop(semaforoConfig, &liberar, 1); //Libreo el semaforo
            flag_config = false;
        }
        sleep(1);
    }
}



/**
 * @fn void SIGINT_handler(int sig) 
 * @details 
 * @param sig
 * @return void
**/
void SIGINT_handler(int sig)
{
    flag_exit = true;
    printf("|SIGINT ID=%d\r\n", getpid());
}


void SIGUSR2_handler(int sig)
{
    flag_config = true;
    //actualizar configuracion
    return;
}

void SIGCHLD_handler(int sig)
{
    pid_t deadchild = 1;
    while (deadchild > 0)
    {
        deadchild = waitpid(-1, NULL, WNOHANG);
        if (deadchild > 0)
        {
            printf("|Murio el hijo ID=%d\r\n", deadchild);
        }
    }
}


void configuracion_reload( config_t *serverConf)
{
    t_config *config;

    //Leo el archivo de configuracion
    config = config_create(CONFIG_RUTA);
    if (config != NULL)
    {
        //El archivo existe, lo leo.
        if (config_has_property(config, "CONEXIONES_MAX") == true)
        {
            serverConf->conexiones_max = config_get_int_value(config, "CONEXIONES_MAX");
        }
        else
        {
            serverConf->conexiones_max = CANT_CONEX_MAX_DEFAULT;
        }
        if (config_has_property(config, "MUESTREO_SENSOR") == true)
        {
            serverConf->muestreo = config_get_int_value(config, "MUESTREO_SENSOR");
        }
        else
        {
            serverConf->muestreo = MUESTREO_SENSOR;
        }
    }
    else
    {
        //No existe el archivo de configuracion, utilizo valores por default
        printf("No existe el archivo de configuracion\n");
        serverConf->backlog = BACKLOG_DEFAULT;
        serverConf->conexiones_max = CANT_CONEX_MAX_DEFAULT;
        serverConf->muestreo = MUESTREO_SENSOR;
    }

    printf("*****************************\n");
    printf("Nueva Configuracion cargada  \n");
    printf("    Max Con. =    %d        \n", serverConf->conexiones_max);
    printf("    Backlog  =    %d        \n", serverConf->backlog);
    printf("    Muestras =    %d        \n", serverConf->muestreo);
    printf("*****************************\n");
}