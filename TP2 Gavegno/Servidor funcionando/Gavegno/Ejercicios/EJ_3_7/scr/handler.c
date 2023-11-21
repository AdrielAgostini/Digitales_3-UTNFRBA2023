
#include "../inc/handler.h"

extern int pid;

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
    pid = fork();
}

void SIGINT_handler(int sig)
{
    printf("Soy inmortal %d\r\n", getpid());
}
