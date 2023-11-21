
#include "../inc/define.h"
#include "../inc/handler.h"


//-- Variables Globales --
int pid;

//-- MAIN --
int main (int argc, char *argv[]) 
{
int var = 0;
int i;
int CANTCHLD;

	if(argc > 1)
	{
		CANTCHLD = (int)*argv[1] - '0'; 
	}

	signal(SIGCHLD, SIGCHLD_handler);
	signal(SIGTERM, SIGTERM_handler);
	signal(SIGUSR1, SIGUSR1_handler);
	signal(SIGINT, SIGINT_handler);

	for(i=0;i<10;i++) 
    {
		kill(getpid(), SIGUSR1);

		if(pid == 0)
    	{
			//Código del hijo
        	for(i=0; i < 5; i++)
       		{
            	printf("Proceso hijo ID=%d, mi parde es ID=%d, var=%d\r\n", getpid(), getppid(),i);
            	sleep(5);
        	}
        	kill(getpid(), SIGKILL);
			//exit(0);		
    	}
    }

    //Código del padre
	printf("Proceso padre ID = %d, Variable = %d \n\r", getpid(), var);
		
	sleep(360);
	printf("Muere el padre ID = %d\n\r", getpid());

	return(0);
} 

