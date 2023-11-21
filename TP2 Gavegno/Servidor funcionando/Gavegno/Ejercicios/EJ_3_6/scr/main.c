
#include "../inc/define.h"
#include "../inc/handler.h"

//-- MAIN --
int main (void) 
{
int var = 0;

int i;

	signal(SIGCHLD, SIGCHLD_handler);
	signal(SIGTERM, SIGTERM_handler);
	signal(SIGUSR1, SIGUSR1_handler);

	for(i=0;i<10;i++) 
    {
		kill(getpid(), SIGUSR1);
    }

    //Código del padre
	printf("Proceso padre ID = %d, Variable = %d \n\r", getpid(), var);
		
	sleep(120);
	printf("Muere el padre ID = %d\n\r", getpid());

	return(0);
} 

