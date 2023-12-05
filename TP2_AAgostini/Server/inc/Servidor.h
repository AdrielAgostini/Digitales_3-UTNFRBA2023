#ifndef _SERVIDOR_H_
#define _SERVIDOR_H_


#include "./PR_Sensor.h"
#include "./PR_IPC.h"
#include "../comons/config.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <errno.h>
#include <fcntl.h>
#include <stdint.h>
#include <math.h>
#include <signal.h>
#include <stdbool.h>
#include <sys/wait.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/sem.h>
#include <sys/stat.h>
#include <sys/select.h>
#include <sys/sendfile.h>


// Define SHARED MEMORY
#define SHARED_SIZE             4096

#define KEY_SENSOR   0x1245
#define KEY_CONFIG   0x1232

#define CANT_CONEX_MAX_DEFAULT  100
#define BACKLOG_DEFAULT         20
#define MUESTREO_SENSOR         2

#define CONFIG_RUTA             "./Config/Servidor.cfg"


/*typedef struct confServer {
    int conexiones_max;         
    int backlog;                
    int muestreo;
    int conexiones;               
}config_t;*/


/*---------------Global Variables Begin---------------*/
config_t *configuracionServer;
sensor_t *SensorData ;
void *sharMemSensor ;
void *sharMemConfig ;
int semaforoSensor, semaforoConfig;
struct sembuf tomar ;   // Estructura para tomar el semáforo
struct sembuf liberar ; // Estructura para liberar el semáforo
int flag_exit;
/*---------------Global Variables End---------------*/

void ProcesarCliente(int, struct sockaddr_in *, int );
int Init_IPC (int *, int *);
int Init_Server (int *,int);
void End_IPC (void);
void configuracion_init(config_t *);

void SIGINT_handler(int sig);

void ManejadorConfiguracion(void);
void SIGUSR2_handler(int );
void SIGCHLD_handler(int );
void configuracion_reload( config_t *);







#endif /* _SERVIDOR_H_ */