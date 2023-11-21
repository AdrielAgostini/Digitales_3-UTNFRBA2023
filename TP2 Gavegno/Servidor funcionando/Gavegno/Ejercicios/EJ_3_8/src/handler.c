
#include "../inc/handler.h"

//------------ Extern------------------
extern int pid;
extern int var, varP;
extern int CANTCHLD;

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
			//Código del hijo
            var++;
    	}
		else if(pid > 0)
		{
			//Código del padre

            if(varP < CANTCHLD)
            {
                pid = fork();
                varP++;
            }
                
		}
}

void SIGUSR2_handler(int sig)
{
    FLAG_KILL_PADRE = 1;
}

void SIGINT_handler(int sig)
{
    printf("Soy inmortal %d\r\n", getpid());
}
