
#include "../inc/Servidor.h"
#include "../inc/funcSIMD.h"

#define ARES_2G 2.0f / 32768.0f
#define GYRO_250 250.0f / 32768.0f



sensor_t PromSIMD(uint8_t *data, int samples)
{
    sensor_t auxdata;
    int i = 0;
    float auxAx = 0, auxAy = 0, auxAz = 0, auxGx = 0, auxGy = 0, auxGz = 0, auxT = 0;
    int16_t auxTem = 0, auxAcel = 0, auxGyr = 0;

    while (i < (samples * sizeof(sensorMPU_t)))
    {
        auxAcel = (int16_t)((data[i] << 8) | data[i + 1]);
        auxAx += (float)auxAcel * ARES_2G;
        auxAcel = (int16_t)((data[i + 2] << 8) | data[i + 3]);
        auxAy += (float)auxAcel * ARES_2G;
        auxAcel = (int16_t)((data[i + 4] << 8) | data[i + 5]);
        auxAz += (float)auxAcel * ARES_2G;

        auxTem = (int16_t)((data[i + 6] << 8) | data[i + 7]);
        auxT += ((float)auxTem / 340) + 36.53;

        auxGyr = (int16_t)((data[i + 8] << 8) | data[i + 9]);
        auxGx += (float)auxGyr * GYRO_250;
        auxGyr = (int16_t)((data[i + 10] << 8) | data[i + 11]);
        auxGy += (float)auxGyr * GYRO_250;
        auxGyr = (int16_t)((data[i + 12] << 8) | data[i + 13]);
        auxGz += (float)auxGyr * GYRO_250;

        i += 14;
    }
    i=0;
    auxdata.accel_xout = auxAx / samples;
    auxdata.accel_yout = auxAy / samples;
    auxdata.accel_zout = auxAz / samples;
    auxdata.temp_out = auxT / samples;
    auxdata.gyro_xout = auxGx / samples;
    auxdata.gyro_yout = auxGy / samples;
    auxdata.gyro_zout = auxGz / samples;



    return auxdata;
}