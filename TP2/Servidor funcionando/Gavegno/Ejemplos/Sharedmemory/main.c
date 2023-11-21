#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <sys/shm.h>
#include <sys/wait.h>

#define KEY 0x1111
#define SHM_SIZE 1024 /* 1K shared memory segment */

union semun
{
    int val;
    struct semid_ds *buf;
    unsigned short *array;
};

struct sembuf p = {0, -1, SEM_UNDO}; // Estructura para tomar el semáforo
struct sembuf v = {0, +1, SEM_UNDO}; // Estructura para liberar el semáforo

void SIGCHLD_handler(int sig)
{
pid_t deadchild = 1;
    while (deadchild > 0)
    {
        deadchild = waitpid(-1, NULL, WNOHANG);
        if(deadchild > 0)
        {
            printf("Murio el hijo ID=%d\r\n", deadchild);
        }
    }
}


int main()
{
    void *addr = (void *)0;

    int pid, i;
    int id = semget(KEY, 1, 0666 | IPC_CREAT);           // semget crea 1 semáforo con permisos rw rw rw
    int shmid = shmget(KEY, SHM_SIZE, 0666 | IPC_CREAT); //crear memoria compartida en el kernel de 1024 Byte

    if (id < 0)
    {
        perror("semget");
        exit(11); // Chequea que este bien creado el semáforo
    }
    union semun u; // Crea la union
    u.val = 1;     // Asigna un 1 al val
    // Inicializo el semáforo para utilizarlo
    if (semctl(id, 0, SETVAL, u) < 0)
    {
        perror("semctl");
        exit(12);
    }

    addr = shmat(shmid, NULL, 0); //addr apunta a la memoria compartida
    if (addr == (char *)(-1))
    {
        perror("shmat");
        exit(1);
    }
    
    signal(SIGCHLD, SIGCHLD_handler);

    pid = fork();
    srand(pid); // Inicializa generador de numeros random. Podria haberle pasado cualquier número.
    if (pid < 0)
    {
        perror("fork");
        exit(1);
    }
    else if (pid > 0)
    {
        //Codigo del padre
        for ( i = 0; i < 5; ++i)
        {
            // Toma el semáforo primero
            if (semop(id, &p, 1) < 0)
            {
                perror("semop p");
                exit(13);
            }
            printf("\nIngrese string:\n");

            gets(addr);
            
            //printf("writing to segment: \"%s\"\n", argv[1]);
            //strncpy(addr, argv[1], SHM_SIZE);

            if (semop(id, &v, 1) < 0)
            {
                perror("semop p");
                exit(14);
            }
            sleep(rand() % 2);
        }
        shmdt(addr); //Separo la memoria del proceso
    }
    else
    {
        //Codigo del hijo
        for (int i = 0; i < 5; ++i)
        {
            if (semop(id, &p, 1) < 0)
            {
                perror("semop p");
                exit(15);
            }
            printf("\nContenido de la memoria:\n%s\n", addr);

            if (semop(id, &v, 1) < 0)
            {
                perror("semop p");
                exit(16);
            }
            sleep(rand() % 2);
        }
        shmdt(addr); //Separo la memoria del proceso
        exit(0);
    }

    wait(NULL);
    semctl(id, 0, IPC_RMID);
    shmctl(shmid, IPC_RMID, NULL);
}
