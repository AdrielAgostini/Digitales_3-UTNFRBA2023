
#include "../inc/define.h"
#include "../inc/handler.h"

//-- MAIN --
int main (void) 
{
int var = 0;
int pid;
int i;

	for(i=0;i<5;i++) 
    {
		pid = fork();
        if(pid == 0)
        {
			//Código del hijo
			var++;
			printf("Proceso hijo ID=%d y mi padre es %d\n\rVariable = %d\r\n", getpid(), getppid(),var);
			
			sleep(5);
			kill(getpid(), SIGKILL);
        }
		else if(pid < 0)
		{
			//ERROR
		}
    }
	if(pid > 0) 
	{
    	//Código del padre
		printf("Proceso padre ID = %d, Variable = %d \n\r", getpid(), var);
		sleep(30);
	}

	return(0);
} 

