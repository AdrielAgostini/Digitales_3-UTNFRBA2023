#include "../inc/PR_Sensor.h"
#include "../inc/Servidor.h"
#include "../inc/funcSIMD.h"

extern config_t *configuracionServer;
extern sensor_t *SensorData;
extern int semaforoConfig;
extern int semaforoSensor;
extern int flag_exit;


void Sensor_readData(void)
{
    sensor_t auxDatos;
    int auxMuestreo = 0;
    static int sen_fd;
    uint8_t *bufferMPU6050;
    uint8_t estado;

    printf("|Manejador del Sensor\n");



    while (!flag_exit)
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
            sen_fd = open("/dev/acelerometro-td3", O_RDWR);
            if (sen_fd < 0)
            {
                perror("No puedo abrir acelerometro-td3\n");
                exit(1);
            }
            estado = read(sen_fd, bufferMPU6050, auxMuestreo*sizeof(sensorMPU_t));
            
            if( estado != 0)
            {
                //Error en la lectura
                printf("Error en la lectura en el Manejador de Sensor\n");
            }
            
            else
            {
                
                auxDatos = PromSIMD( bufferMPU6050, auxMuestreo);
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
        sleep(3);
    }close (sen_fd);
    sleep(3);
    return;
}

