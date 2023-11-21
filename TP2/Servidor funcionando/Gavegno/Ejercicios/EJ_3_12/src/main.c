
#include "../inc/define.h"
#include "../inc/handler.h"
#include "../inc/init.h"

extern int pid;

//-- MAIN --
int main(int argc, char *argv[])
{
	char exitwhile = 1;

	init_signal();

	while (exitwhile)
	{
		if (pid == 0)
		{
			//Código del hijo
			//execlp("sleep", "sleep", "10s", (char *)NULL);			
			system("sleep");

			//sleep(5);
		}
		else if (pid > 0)
		{
			//Código del padre
			sleep(5);
			if (FLAG_KILL_PADRE == 1)
			{
				exitwhile = 0;
			}
			//printf("Proceso padre ID = %d\n\r", getpid());
		}
	}

	printf("Muere el padre ID = %d\n\r", getpid());

	return (0);
}
