
#include "../inc/handler.h"


extern int pid;
extern int pid2;

extern int FLAG_SIG1;
extern int FLAG_SIG2;

extern int FLAG_CHILD_READ;


//-------------Variables Globales-------------
int FLAG_KILL_PADRE = 0;

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

void SIGTERM_handler(int sig)
{
    printf("handler SIGTERM %d\r\n", getpid());
}

void SIGUSR1_handler(int sig)
{
    if(pid == 0)
    {
        //codigo del hijo1
        printf("Hijo ID=%d SIGUSR1\r\n", getpid());
        FLAG_CHILD_READ = 1;
    }
    else
    {
        printf("Envio señal al hijo %d SIGUSR1 Padre ID=%d\r\n", pid2, getpid());
        FLAG_SIG1 = 1;
    }

}

void SIGUSR2_handler(int sig)
{

    if(pid2 == 0)
    {
        //codigo del hijo2
        printf("Hijo ID=%d SIGUSR2\r\n", getpid());
        FLAG_CHILD_READ = 1;
    }
    else
    {
        printf("Envio señal al hijo %d SIGUSR2 Padre ID=%d\r\n", pid2, getpid());
        FLAG_SIG2 = 1;
    }
}

void SIGINT_handler(int sig)
{
    printf("Soy inmortal %d\r\n", getpid());
}

void SIGKILL_handler(int sig)
{
    FLAG_KILL_PADRE = 1;
}
