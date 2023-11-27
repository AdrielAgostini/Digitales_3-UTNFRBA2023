
#include "../inc/Servidor.h"
#include "../inc/funcSIMD.h"

#define ARES_2G 2.0f / 32768.0f
#define GYRO_250 250.0f / 32768.0f


/**
 * @fn sensor_t PromedioSIMD(uint8_t *datos, int cant)
 * @details Calcula el promedio 
 * @param datos datos a promediar
 * @param cant  cantidad de datos a promediar
 * @return sensor_t retorna la estructura con el promedio
**/
sensor_t PromedioSIMD(uint8_t *datos, int cant)
{
    sensor_t auxDatos;
    int i = 0;
    float auxAx = 0, auxAy = 0, auxAz = 0, auxGx = 0, auxGy = 0, auxGz = 0, auxT = 0;
    int16_t auxTem = 0, auxAcel = 0, auxGyr = 0;

    while (i < (cant * sizeof(sensorMPU_t)))
    {
        auxAcel = (int16_t)((datos[i] << 8) | datos[i + 1]);
        auxAx += (float)auxAcel * ARES_2G;
        auxAcel = (int16_t)((datos[i + 2] << 8) | datos[i + 3]);
        auxAy += (float)auxAcel * ARES_2G;
        auxAcel = (int16_t)((datos[i + 4] << 8) | datos[i + 5]);
        auxAz += (float)auxAcel * ARES_2G;

        auxTem = (int16_t)((datos[i + 6] << 8) | datos[i + 7]);
        auxT += ((float)auxTem / 340) + 36.53;

        auxGyr = (int16_t)((datos[i + 8] << 8) | datos[i + 9]);
        auxGx += (float)auxGyr * GYRO_250;
        auxGyr = (int16_t)((datos[i + 10] << 8) | datos[i + 11]);
        auxGy += (float)auxGyr * GYRO_250;
        auxGyr = (int16_t)((datos[i + 12] << 8) | datos[i + 13]);
        auxGz += (float)auxGyr * GYRO_250;

        i += 14;
    }
    i=0;
    auxDatos.accel_xout = auxAx / cant;
    auxDatos.accel_yout = auxAy / cant;
    auxDatos.accel_zout = auxAz / cant;
    auxDatos.temp_out = auxT / cant;
    auxDatos.gyro_xout = auxGx / cant;
    auxDatos.gyro_yout = auxGy / cant;
    auxDatos.gyro_zout = auxGz / cant;

    auxAx = 0;
    auxAy = 0;
    auxAz = 0;
    auxT = 0;
    auxGx = 0;
    auxGy = 0;
    auxGz = 0;
/*
    printf("\n*****************************************\n");
    printf("Valor de Acelerometro X en servidor: %.02fg\n", auxDatos.accel_xout);
    printf("Valor de Acelerometro Y en servidor: %.02fg\n", auxDatos.accel_yout);
    printf("Valor de Acelerometro Z en servidor: %.02fg\n", auxDatos.accel_zout);
    printf("Valor de Temperatura en servidor:    %.02f°\n", auxDatos.temp_out);
    printf("Valor de Gyroscopio X en servidor:   %.02f°/seg\n", auxDatos.gyro_xout);
    printf("Valor de Gyroscopio Y en servidor:   %.02f°/seg\n", auxDatos.gyro_yout);
    printf("Valor de Gyroscopio Z en servidor:   %.02f°/seg\n", auxDatos.gyro_zout);
    printf("******************************************\n\n");
*/
    return auxDatos;
}