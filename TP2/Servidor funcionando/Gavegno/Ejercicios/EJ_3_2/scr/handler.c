
#include "../inc/handler.h"

void SIGINT_handler(int sig)
{

}

void SIGUSR1_handler(int sig)
{
    printf("Proceso hijo ID=%d mandó SIGUSR1 \r\n", getpid());
}