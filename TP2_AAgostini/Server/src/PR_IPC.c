#include "../inc/PR_IPC.h"
#include "../inc/Servidor.h"

int sem_create(int *semaforo, key_t key)
{
    *semaforo = semget(key, 1, 0666 | IPC_CREAT); // semget crea 1 semáforo con permisos rw rw rw
    if (*semaforo < 0)
    {
        return -1;
    }
    union semun u; // Crea la union
    u.val = 1;     // Asigna un 1 al val
    // Inicializo el semáforo para utilizarlo
    if (semctl(*semaforo, 0, SETVAL, u) < 0)
    {
        return -1;
    }

    return 0;
}

void *shmem_create(int *shaMemID, key_t key)
{
    void *ret = (void *)0;
    *shaMemID = shmget(key, sizeof(struct confServer), 0666 | IPC_CREAT);
    if (*shaMemID == -1)
    {
        ret = (void *)-1;
    }
    //addr apunta a la memoria compartida
    ret = shmat(*shaMemID, (void *)0, 0);

    return ret;
}