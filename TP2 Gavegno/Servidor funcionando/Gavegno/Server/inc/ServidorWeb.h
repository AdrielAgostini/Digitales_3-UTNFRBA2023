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

union semun {
    int val;
    struct semid_ds *buf;
    unsigned short *array;
    struct seminfo  *__buf;
};

typedef struct confServer {
    int conexiones_max;         
    int backlog;                
    int muestreo;
    int conexiones;               
}config_t;


typedef struct MPU_REGS { 
    int16_t accel_xout;
    int16_t accel_yout;
    int16_t accel_zout;
    int16_t temp_out;
    int16_t gyro_xout;
    int16_t gyro_yout;
    int16_t gyro_zout;
} sensorMPU_t;


typedef struct MPU6050_REGS { 
    float accel_xout;
    float accel_yout;
    float accel_zout;
    float temp_out;
    float gyro_xout;
    float gyro_yout;
    float gyro_zout;
} sensor_t;


void ManejadorSensor(void);
void ProcesarCliente(int s_aux, struct sockaddr_in *pDireccionCliente, int puerto);
void ManejadorConfiguracion(void);
// Signal handlers.
void SIGINT_handler(int sig);
void SIGUSR1_handler(int sig);
void SIGUSR2_handler(int sig);
void SIGCHLD_handler(int sig);

// Signal handlers sensor
void SIGINT_handler_Sensor(int signbr);
