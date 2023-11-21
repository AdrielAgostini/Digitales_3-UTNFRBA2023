
//#include "../inc/define.h"
#include "../inc/handler.h"
#include "../inc/init.h"

//-------Variables Externas--------------

//----- Variables Globales -----
char exitwhile = 1;

char buffer[200];
int pipebuff[2];

int idchld[MAXCHLD];

//----- FLAGS ------------
int FLAG_FIRST = 1;
int FLAG_CLOSE = 0;

//-- MAIN --
int main(int argc, char *argv[])
{
	char exitwhile = 1;

	int pid = 1;
	int pid2 = 1;
	int process = 0;

	char buf[200];
	unsigned char sizebufHijo;
	unsigned char sizebufPadre;

	init_signal();

	pipe(pipebuff);

	pid = fork();

	if (pid == 0)
	{
		//codigo del hijo1
		process = 1;
		printf("Proseso hijo ID=%d, Nº de proceso = %d\r\n", getpid(), process);
		sleep(3);
		close(pipebuff[0]); /* CIERRO EXTREMO DE LECTURA */
	}
	else if (pid > 0)
	{
		//Código del padre
		pid2 = fork();
		if (pid2 == 0)
		{
			//codigo del hijo2
			process = 2;
			close(pipebuff[1]); /* CIERRO EXTREMO DE ESCRITURA */
			printf("Proseso hijo ID=%d, Nº de proceso = %d\r\n", getpid(), process);
		}
	}

	while (exitwhile)
	{
		switch (process)
		{
		case 0:
			//codigo del padre
			if (FLAG_KILL_PADRE == 1)
			{
				exitwhile = 0;
			}
			sleep(5);

			break;

		case 1:
			//codigo del hijo1
			//getc(buf);
			printf("Hijo ID=%d lee: \r\n", getpid());
			read(0, buf, 200);

			sizebufHijo = (unsigned char)strlen(buf);
			write(pipebuff[1], &sizebufHijo, 1);  /* ENVIO LONGITUD DEL BUFFER */
			write(pipebuff[1], buf, sizebufHijo); /* ENVIO BUFFER */
			//printf("Proceso hijo ID=%d, mi padre es ID=%d, mi grupo ID=%d, var=%d\r\n", getpid(), getppid(), getpgrp(),var);
			//sleep(5);

			break;

		case 2:
			//codigo del hijo2
			read(pipebuff[0], &sizebufPadre, 1);	 /* LEO LONGITUD DEL BUFFER */
			read(pipebuff[0], buffer, sizebufPadre); /* LEO BUFFER */
			printf("Hijo ID=%d escibe:%s \r\n", getpid(), buffer);

			break;

		default:
			break;
		}
	}

	printf("Muere el padre ID = %d\n\r", getpid());

	return (0);
}
