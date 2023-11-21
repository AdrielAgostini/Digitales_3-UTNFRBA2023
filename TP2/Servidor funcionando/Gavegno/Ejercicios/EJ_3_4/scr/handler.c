
#include "../inc/handler.h"

void SIGCHLD_handler(int sig)
{
    while (waitpid(-1, NULL, WNOHANG) != 0)
    {
        printf("handler SIGCHLD %d\r\n", getpid());
    }
}

