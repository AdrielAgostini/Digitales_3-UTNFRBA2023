
//#include "../inc/define.h"
#include "../inc/handler.h"
#include "../inc/init.h"

#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
//-------Variables Externas--------------

//----- Variables Globales -----
char buffer[200];

char *MyFIFO = "myFIFO";

//----- FLAGS ------------

//-- MAIN --
int main(int argc, char *argv[])
{
	char exitwhile = 1;
	int i;
	int pid = 1;
	int process = 0;

	int pFile;
	char ArgProcess = 0;

	char buf[100];

	if (argc > 1)
	{
		ArgProcess = *argv[1];
	}

	init_signal();

	mkfifo( MyFIFO, 0666);

	pid = fork();

	if (pid == 0)
	{
		//codigo del hijo1
		process = 1;
		printf("Proceso hijo ID=%d, Tipo de proceso %c\r\n", getpid(), ArgProcess);
	}
	else if (pid > 0)
	{
		//Código del padre
	}

	while (exitwhile)
	{
		switch (process)
		{
		case 0:
			//codigo del padre
			sleep(10);
			if (FLAG_KILL_PADRE == 1)
			{
				kill(pid, SIGKILL);
				exitwhile = 0;
			}

			break;

		case 1:
			//codigo del hijo1

			if (ArgProcess == 'W')
			{
				//pFile = open("mypipe.txt", "w");
				pFile = open( MyFIFO, O_WRONLY);

				//fgets(0, 1,pFile);
				read(0, buf, 100);
				write(pFile, buf, 100);
				//fwrite(buf, strlen(buf), 1, pFile);
				for (i = 0; i < 100; i++)
				{
					buf[i] = '\0';
				}
			}
			else if (ArgProcess == 'R')
			{
				pFile = open( MyFIFO, O_RDONLY);

				read(pFile, buf, 100);
				printf("%s\n", buf);

				for (i = 0; i < 100; i++)
				{
					buf[i] = '\0';
				}

			}
			close(pFile);

			sleep(1);

			break;

		default:
			break;
		}
	}

	printf("Muere el padre ID = %d\n\r", getpid());

	return (0);
}
