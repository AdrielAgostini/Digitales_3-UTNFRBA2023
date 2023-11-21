
#include "../inc/ServidorWeb.h"
#include "../inc/funciones.h"
#include "../inc/configuracion.h"
#include "../inc/funcSIMD.h"

/*---------------------FLAGS-------------------*/
bool FLAG_CONEXION = false;
bool FLAG_CONF = false;
bool FLAG_EXIT = false;

/*---------------Variables Globales----------------*/
int semaforoSensor, semaforoConfig;
config_t *configuracionServer;
sensor_t *SensorData;
struct sembuf tomar = {0, -1, SEM_UNDO};   // Estructura para tomar el semáforo
struct sembuf liberar = {0, +1, SEM_UNDO}; // Estructura para liberar el semáforo

uint32_t cantHijos = 0;

/***********************************************************
* MAIN                                           
* Parametro: Numero de Puerto                               
***********************************************************/
int main(int argc, char *argv[])
{
    int sock;
    int sensorPID, confPID, padrePID;
    struct sockaddr_in datosServidor;
    socklen_t longDirec;
    int sharMemIdSensor, sharMemIdConfig;
    void *sharMemSensor = (void *)0;
    void *sharMemConfig = (void *)0;
    struct timeval timeout;
    int auxBacklog = 0, auxConexMAX = 0, auxConexiones = 0;

    if (argc != 2)
    {
        printf("\n\nFalta especificar el PUERTO\n\n");
        exit(1);
    }

    // Signal handlers
    signal(SIGINT, SIGINT_handler);
    signal(SIGCHLD, SIGCHLD_handler);
    signal(SIGUSR1, SIGUSR1_handler);
    signal(SIGUSR2, SIGUSR2_handler);

    padrePID = getpid();

    /************************************************************
*                   INICIALIZACION                          *
*************************************************************/

    // Creo la Shared memory para el sensor
    sharMemSensor = creo_SharedMemory(&sharMemIdSensor, KEY_SENSOR);
    if (sharMemSensor == (void *)-1)
    {
        printf("|ERROR: No se pudo obtener Shared Memory para el sensor\n");
        return -1;
    }

    // Creo la Shared memory para la configuracion
    sharMemConfig = creo_SharedMemory(&sharMemIdConfig, KEY_CONFIG);
    if (sharMemConfig == (void *)-1)
    {
        printf("|ERROR: No se pudo obtener Shared Memory para la Configuracion\n");
        return -1;
    }

    //Apunto directamente a la SharedMemory
    SensorData = (sensor_t *)sharMemSensor;
    configuracionServer = (config_t *)sharMemConfig;

    //Creacion del semaforo para el sensor
    if (crear_semaforo(&semaforoSensor, KEY_SENSOR) < 0)
    {
        perror("|ERROR: No se pudo crear el semaforo para el sensor\n");
        semctl(semaforoSensor, 0, IPC_RMID);     //Cierro el semaforo del sensor
        shmdt(sharMemSensor);                    //Separo la memoria del sensor del proceso
        shmdt(sharMemConfig);                    //Separo la memoria de la configuracion del proceso
        shmctl(sharMemIdSensor, IPC_RMID, NULL); //Cierro la Shared Memory del sensor
        shmctl(sharMemIdConfig, IPC_RMID, NULL); //Cierro la Shared Memory de la configuracion
        exit(1);
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
        exit(1);
    }

    printf("|*******************************************************\n");
    printf("|                                                       \n");
    printf("|Proceso Padre ID                   = [%d]              \n", padrePID);
    printf("|Semaforo Sensor                    = [%d]              \n", semaforoSensor);
    printf("|SharedMemory Sensor ID             = [%d]              \n", sharMemIdSensor);
    printf("|SharedMemory Sensor ADDRES         = [%p]              \n", sharMemSensor);
    printf("|Semaforo Configuracion             = [%d]              \n", semaforoConfig);
    printf("|SharedMemory Configuracion ID      = [%d]              \n", sharMemIdConfig);
    printf("|SharedMemory Configuracion ADDRES  = [%p]              \n", sharMemConfig);
    printf("|                                                       \n");
    printf("|*******************************************************\n");

    //--------Configuracion del Server----------
    semop(semaforoConfig, &tomar, 1); //Tomo el semaforo
    configuracion_init(configuracionServer);
    semop(semaforoConfig, &liberar, 1); //Libreo el semaforo

    /********************************************************
*                   CREO EL SERVIDOR                    *
*********************************************************/
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

    // child que controla es sensor
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
        signal(SIGINT, SIGINT_handler_Sensor);
        ManejadorSensor();

        semctl(semaforoSensor, 0, IPC_RMID);     //Cierro el semaforo del sensor
        shmdt(sharMemSensor);                    //Separo la memoria del sensor del proceso
        shmctl(sharMemIdSensor, IPC_RMID, NULL); //Cierro la Shared Memory del sensor
        close(sock);
        exit(1);
    }
    cantHijos++;
    printf("|*******************************************************\n");
    printf("|                                                       \n");
    printf("|Proceso Sensor ID = %d                                 \n", sensorPID);
    printf("|                                                       \n");
    printf("|*******************************************************\n");
    // child configuracion
    confPID = fork();
    if (confPID < 0)
    {
        semctl(semaforoConfig, 0, IPC_RMID);     //Cierro el semaforo de la configuracion
        shmdt(sharMemConfig);                    //Separo la memoria de la configuracion del proceso
        shmctl(sharMemIdConfig, IPC_RMID, NULL); //Cierro la Shared Memory de la configuracion
        close(sock);
        exit(1);
    }
    if (confPID == 0)
    {
        //Funcion para manejar la configuracion
        ManejadorConfiguracion();

        semctl(semaforoConfig, 0, IPC_RMID);     //Cierro el semaforo de la configuracion
        shmdt(sharMemConfig);                    //Separo la memoria de la configuracion del proceso
        shmctl(sharMemIdConfig, IPC_RMID, NULL); //Cierro la Shared Memory de la configuracion
        close(sock);
        exit(1);
    }
    cantHijos++;
    printf("|*******************************************************\n");
    printf("|                                                       \n");
    printf("|Proceso Config ID = %d                                 \n", confPID);
    printf("|                                                       \n");
    printf("|*******************************************************\n");

    // Permite atender a multiples usuarios
    while (!FLAG_EXIT)
    {
        int clientePID, sock_aux;
        struct sockaddr_in datosCliente;
        int nbr_fds;
        fd_set readfds;

        /********************************************************************************
                    Pregunto por la cantidad de conexiones
           y limito nuevas conexiones si alcanze el maximo permitido
*********************************************************************************/
        semop(semaforoConfig, &tomar, 1);                  //Tomo el semaforo
        auxConexMAX = configuracionServer->conexiones_max; //Cantidad de conexiomes permitidas
        auxConexiones = configuracionServer->conexiones;   //conexiones actuales
        semop(semaforoConfig, &liberar, 1);                //Libreo el semaforo

        if (auxConexiones < auxConexMAX)
        {
            // Crear la lista de "file descriptors" que vamos a escuchar
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
            if (nbr_fds > 0)
            {
                // La funcion accept rellena la estructura address con
                // informacion del cliente y pone en longDirec la longitud
                // de la estructura.
                longDirec = sizeof(datosCliente);
                sock_aux = accept(sock, (struct sockaddr *)&datosCliente, &longDirec);
                if (sock_aux < 0)
                {
                    perror("|Error en accept");
                    semctl(semaforoSensor, 0, IPC_RMID);     //Cierro el semaforo del sensor
                    shmdt(sharMemSensor);                    //Separo la memoria del sensor del proceso
                    shmctl(sharMemIdSensor, IPC_RMID, NULL); //Cierro la Shared Memory del sensor
                    semctl(semaforoConfig, 0, IPC_RMID);     //Cierro el semaforo de la configuracion
                    shmdt(sharMemConfig);                    //Separo la memoria de la configuracion del proceso
                    shmctl(sharMemIdConfig, IPC_RMID, NULL); //Cierro la Shared Memory de la configuracion
                    close(sock);
                    exit(1);
                }

                clientePID = fork();
                if (clientePID < 0)
                {
                    perror("|No se puede crear un nuevo proceso mediante fork");
                    close(sock);
                    exit(1);
                }cantHijos++;
                if (clientePID == 0)
                {                                     // Proceso hijo.
                    semop(semaforoConfig, &tomar, 1); //Tomo el semaforo
                    configuracionServer->conexiones++;
                    semop(semaforoConfig, &liberar, 1); //Libreo el semaforo

                    printf("|*******************************************************\n");
                    printf("|                                                       \n");
                    printf("|Proceso Cliente ID = %d                                \n", getpid());
                    printf("|                                                       \n");
                    printf("|*******************************************************\n");
                    ProcesarCliente(sock_aux, &datosCliente, atoi(argv[1]));

                    semop(semaforoConfig, &tomar, 1); //Tomo el semaforo
                    configuracionServer->conexiones--;
                    semop(semaforoConfig, &liberar, 1); //Libreo el semaforo

                    exit(0);
                }
                close(sock_aux); // El proceso padre debe cerrar el socket que usa el hijo.
            }
        }
    }

    // Esperando a que mueran los hijos para luego matar al padre.
    while(1) 
    {
        printf("|----------------------------------------------------\n");
        printf("|SERVIDOR: Esperando que mueran los hijos...\n");
        printf("|----------------------------------------------------\n");
        if(cantHijos == 0)
        {
            close(sock);
            semctl(semaforoSensor, 0, IPC_RMID);     //Cierro el semaforo del sensor
            shmdt(sharMemSensor);                    //Separo la memoria del sensor del proceso
            shmctl(sharMemIdSensor, IPC_RMID, NULL); //Cierro la Shared Memory del sensor
            semctl(semaforoConfig, 0, IPC_RMID);     //Cierro el semaforo de la configuracion
            shmdt(sharMemConfig);                    //Separo la memoria de la configuracion del proceso
            shmctl(sharMemIdConfig, IPC_RMID, NULL); //Cierro la Shared Memory de la configuracion
            
            printf("|----------------------------------------------------\n");
            printf("|SERVIDOR: Murio el padre\n");
            printf("|----------------------------------------------------\n");
            return 0;
        }
        sleep(1);
    }
   
}

/**
 * @fn void ProcesarCliente(int s_aux, struct sockaddr_in *pDireccionCliente, int puerto)
 * @brief 
 * @details 
 * @param s_aux
 * @param pDireccionCliente puntero a la direccion IP del servidor 
 * @param puerto Puerto utilizado por el servidor
 * @return void
**/
void ProcesarCliente(int s_aux, struct sockaddr_in *pDireccionCliente, int puerto)
{
    char bufferComunic[8162]; //8KB       antes 4KB 4096
    char ipAddr[20];
    int Port;
    char HTML[4096];           //1KB       antes 4KB 4096
    char encabezadoHTML[1024]; //1KB       antes 4KB 4096

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
    sprintf(encabezadoHTML, "<html><head><title>ServerWeb</title>"
                            "<meta name=\"viewport\" "
                            "content=\"width=device-width, initial-scale=2.0\">"
                            "</head>"
                            "<h1>ServerWeb</h1>");

    sprintf(HTML,
            "%s<p> Servidor Web.</p>"
            "<p> Valores del sensor MPU6050: </p>"
            "<p> accel_Xout = %.02fg </p>"
            "<p> accel_Yout = %.02fg </p>"
            "<p> accel_Zout = %.02fg </p>"
            "<p> temp_out   = %.02f° </p>"
            "<p> gyro_Xout  = %.02f°/s </p>"
            "<p> gyro_Yout  = %.02f°/s </p>"
            "<p> gyro_Zout  = %.02f°/s </p>",
            encabezadoHTML, SensorData->accel_xout, SensorData->accel_yout, SensorData->accel_zout,
            SensorData->temp_out, SensorData->gyro_xout, SensorData->gyro_yout, SensorData->gyro_zout);

    sprintf(bufferComunic,
            "HTTP/1.1 200 OK\n"
            "Content-Length: %ld\n"
            "Content-Type: text/html; charset=utf-8\n"
            "Connection: Closed\n\n%s",
            strlen(HTML), HTML);
    semop(semaforoSensor, &liberar, 1); //Libreo el semaforo

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

/**
 * @fn void ManejadorSensor(void)
 * @brief 
 * @details 
 * @param void
 * @return void
**/
void ManejadorSensor(void)
{
    sensor_t auxDatos;
    int auxMuestreo = 0;
    static int sen_fd;
    uint8_t *bufferMPU6050;
    uint8_t estado;

    printf("|Manejador del Sensor\n");

    sen_fd = open("/dev/acelerometro-td3", O_RDWR);
    if (sen_fd < 0)
    {
        perror("No puedo abrir acelerometro-td3\n");
        exit(1);
    }

    while (!FLAG_EXIT)
    {
        semop(semaforoConfig, &tomar, 1); //Tomo el semaforo
        auxMuestreo = configuracionServer->muestreo;
        semop(semaforoConfig, &liberar, 1); //Libreo el semaforo

        bufferMPU6050 = malloc(auxMuestreo * sizeof(sensorMPU_t));
        if(bufferMPU6050 == NULL)
        {
            //No pudo reservar memoria 
            printf("No pudo reservar memoria en el Manejador de Sensor\n");
        }
        else
        {
            estado = read(sen_fd, bufferMPU6050, auxMuestreo*sizeof(sensorMPU_t));
            if( estado != 0)
            {
                //Error en la lectura
                printf("Error en la lectura en el Manejador de Sensor\n");
            }
            else
            {
                auxDatos = PromedioSIMD( bufferMPU6050, auxMuestreo);

                semop(semaforoSensor, &tomar, 1); //Tomo el semaforo
                SensorData->accel_xout = auxDatos.accel_xout;
                SensorData->accel_yout = auxDatos.accel_yout;
                SensorData->accel_zout = auxDatos.accel_zout;
                SensorData->temp_out = auxDatos.temp_out;
                SensorData->gyro_xout = auxDatos.gyro_xout;
                SensorData->gyro_yout = auxDatos.gyro_yout;
                SensorData->gyro_zout = auxDatos.gyro_zout;
                semop(semaforoSensor, &liberar, 1); //Libreo el semaforo
            }
            free(bufferMPU6050);
        }
        sleep(0.5);
    }

    close(sen_fd);
    return;
}

/**
 * @fn void ManejadorConfiguracion(void)
 * @details 
 * @param void
 * @return void
**/
void ManejadorConfiguracion(void)
{
    printf("Manejador de la configuracion\n");

    while (!FLAG_EXIT)
    {
        if (FLAG_CONF == true)
        {
            //--------Configuracion del Server----------
            semop(semaforoConfig, &tomar, 1); //Tomo el semaforo
            configuracion_reload(configuracionServer);
            semop(semaforoConfig, &liberar, 1); //Libreo el semaforo
            FLAG_CONF = false;
        }
        sleep(0.5);
    }
}

/**
 * @fn void SIGINT_handler_Sensor(int sig)
 * @details 
 * @param sig
 * @return void
**/
void SIGINT_handler_Sensor(int sig)
{
    FLAG_EXIT = true;
    printf("|SIGINT sensor ID=%d\r\n", getpid());
}

/**
 * @fn void SIGINT_handler(int sig) 
 * @details 
 * @param sig
 * @return void
**/
void SIGINT_handler(int sig)
{
    FLAG_EXIT = true;
    printf("|SIGINT ID=%d\r\n", getpid());
}

/**
 * @fn void SIGUSR1_handler(int sig)
 * @details 
 * @param sig
 * @return void
**/
void SIGUSR1_handler(int sig)
{

}

/**
 * @fn void SIGUSR2_handler(int sig)
 * @details 
 * @param sig
 * @return void
**/
void SIGUSR2_handler(int sig)
{
    FLAG_CONF = true;
    //actualizar configuracion
    return;
}

/**
 * @fn void SIGCHLD_handler(int sig)
 * @details 
 * @param sig
 * @return void
**/
void SIGCHLD_handler(int sig)
{
    pid_t deadchild = 1;
    while (deadchild > 0)
    {
        deadchild = waitpid(-1, NULL, WNOHANG);
        if (deadchild > 0)
        {
            cantHijos--;
            printf("|Murio el hijo ID=%d\r\n", deadchild);
        }
    }
}
