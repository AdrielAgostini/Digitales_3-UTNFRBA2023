#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <math.h>

#include "../inc/ServidorWeb.h"
#include "../inc/funciones.h"

extern config_t *configuracionServer;
extern int semaforoID;
extern int semaforoSensor, semaforoConfig;

extern struct sembuf tomar; // Estructura para tomar el semáforo
extern struct sembuf liberar; // Estructura para liberar el semáforo


int crear_semaforo(int *semaforo, key_t key)
{
    int retorno = 0;
    *semaforo = semget(key, 1, 0666 | IPC_CREAT); // semget crea 1 semáforo con permisos rw rw rw
    if (*semaforo < 0)
    {
        retorno = -1;
    }
    union semun u; // Crea la union
    u.val = 1;     // Asigna un 1 al val
    // Inicializo el semáforo para utilizarlo
    if (semctl(*semaforo, 0, SETVAL, u) < 0)
    {
        retorno = -1;
    }

    return retorno;
}

void *creo_SharedMemory(int *shaMemID, key_t key)
{
    void *retorno = (void *)0;
    *shaMemID = shmget(key, sizeof(struct confServer), 0666 | IPC_CREAT);
    if (*shaMemID == -1)
    {
        retorno = (void *)-1;
    }
    //addr apunta a la memoria compartida
    retorno = shmat(*shaMemID, (void *)0, 0);

    return retorno;
}
