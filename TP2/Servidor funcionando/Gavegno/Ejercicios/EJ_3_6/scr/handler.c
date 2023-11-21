
#include "../inc/handler.h"

void SIGCHLD_handler(int sig)
{
    while (waitpid(-1, NULL, WNOHANG) != 0)
    {
        printf("handler SIGCHLD %d\r\n", getpid());
    }
}

void SIGTERM_handler(int sig)
{
    printf("handler SIGTERM %d\r\n", getpid());
}

void SIGUSR1_handler(int sig)
{
int pid;
static int cantPid=0;

    if(cantPid < 10)
    {
        pid= fork();
        cantPid++;
    }
    if(pid == 0)
    {
		//Código del hijo
		printf("Proceso hijo ID=%d\r\n", getpid());
		
		sleep(60);
		exit(0);		
    }
}
