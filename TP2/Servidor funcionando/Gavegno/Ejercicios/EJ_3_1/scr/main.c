#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>

//-- MAIN --
int main (void) 
{
int var = 0;
int pid;

	pid = fork();
	if (pid == 0) 
	{
		//Código del hijo
		var++;
		printf("Proceso hijo ID = %d, Variable = %d \n\r", getpid(), var);
		sleep(5);
	}
	else if(pid > 0) 
	{
	    //Código del padre
		printf("Proceso padre ID = %d, Variable = %d \n\r", getpid(), var);
		sleep(5);
	}
	else 
	{
	    //Error		
    }

	return(0);
} 

