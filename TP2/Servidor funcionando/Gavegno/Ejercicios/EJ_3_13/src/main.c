
//#include "../inc/define.h"
#include "../inc/handler.h"
#include "../inc/init.h"

#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
//-------Variables Externas--------------

//----- Variables Globales -----
int pid = 1, pid2 = 1;

char buffer[100];

char *texto = "";

//----- FLAGS ------------
int FLAG_SIG1 = 0;
int FLAG_SIG2 = 0;

int FLAG_CHILD_READ = 0;
int FLAG_CHILD_WRITE = 0;

//-- MAIN --
int main(int argc, char *argv[])
{
	int contador=0, first = 0;
	int process = 0;
	char exitwhile = 1;
	char errBuf[256];
	long lSize;
	FILE *pFile1;
	FILE *pFile2;

	init_signal();

	pid = fork();

	if (pid == 0)
	{
		//codigo del hijo1
		process = 1;
		printf("Proceso hijo ID=%d, Nº de proceso %d\r\n", getpid(), process);
	}
	else if (pid > 0)
	{
		//Código del padre
		pid2 = fork();
		if (pid2 == 0)
		{
			//codigo del hijo2
			process = 2;
			printf("Proceso hijo ID=%d, Nº de  proceso %d\r\n", getpid(), process);
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
			sleep(1);

			if (FLAG_SIG1)
			{
				kill(pid, SIGUSR1);
			}

			if (FLAG_SIG2)
			{
				kill(pid2, SIGUSR2);
			}

			break;

		case 1:
			//codigo del hijo1
			sleep(5);
			if (first == 0)
			{
				printf("Hijo1 %d abre archivo por primera vez\r\n \r\n", getpid());

				pFile2 = fopen("data2.txt", "w");
				if (pFile2 == NULL)
				{
					//-- Error al abrir el archivo --
					strerror_r(errno, errBuf, sizeof(errBuf));					   //-- Obtengo la leyenda del error --
					printf("Error al abrir el archivo %d: %s\r\n", errno, errBuf); //-- Imprime en stdout --
					//perror ("Error al abrir el archivo test00.txt");		//-- Imprime en stderror --
					return (-1);
				}
				fwrite("Que la fuerza te acompañe\r\n", 1, sizeof("Que la fuerza te acompañe\r"), pFile2);
				first = 1;
				printf("Hijo1 %d escribio y envia señal SIGUSR2\r\n \r\n", getpid());
				kill(getppid(), SIGUSR2);
				fclose(pFile2);
			}

			if (FLAG_CHILD_READ)
			{
				pFile1 = fopen("data1.txt", "r");
				if (pFile1 == NULL)
				{
					//-- Error al abrir el archivo --
					strerror_r(errno, errBuf, sizeof(errBuf));					   //-- Obtengo la leyenda del error --
					printf("Error al abrir el archivo %d: %s\r\n", errno, errBuf); //-- Imprime en stdout --
					//perror ("Error al abrir el archivo test00.txt");		//-- Imprime en stderror --
					return (-1);
				}
				//obtener el tamaño del archivo
				fseek(pFile1, 0, SEEK_END);
				lSize = ftell(pFile1);
				rewind(pFile1);

				fread(buffer, 1, lSize, pFile1);

				fclose(pFile1);
				FLAG_CHILD_READ = 0;
				FLAG_CHILD_WRITE = 1;
				
			}

			if (FLAG_CHILD_WRITE)
			{
				pFile2 = fopen("data2.txt", "w");
				if (pFile2 == NULL)
				{
					//-- Error al abrir el archivo --
					strerror_r(errno, errBuf, sizeof(errBuf));					   //-- Obtengo la leyenda del error --
					printf("Error al abrir el archivo %d: %s\r\n", errno, errBuf); //-- Imprime en stdout --
					//perror ("Error al abrir el archivo test00.txt");		//-- Imprime en stderror --
					return (-1);
				}

				fwrite(buffer, 1, lSize, pFile2);

				FLAG_CHILD_WRITE = 0;

				fclose(pFile2);

				contador++;

				printf("Hijo1 %d escribio y envia señal SIGUSR2\r\n \r\n", getpid());
				kill(getppid(), SIGUSR2);
			}

			if(contador > 5)
			{
				kill(getpid(), SIGKILL);
			}

			sleep(5);
			break;

		case 2:
			//codigo del hijo2
			sleep(5);
			if (FLAG_CHILD_READ)
			{
				pFile2 = fopen("data2.txt", "r");
				if (pFile2 == NULL)
				{
					//-- Error al abrir el archivo --
					strerror_r(errno, errBuf, sizeof(errBuf));					   //-- Obtengo la leyenda del error --
					printf("Error al abrir el archivo %d: %s\r\n", errno, errBuf); //-- Imprime en stdout --
					//perror ("Error al abrir el archivo test00.txt");		//-- Imprime en stderror --
					return (-1);
				}
				//obtener el tamaño del archivo
				fseek(pFile2, 0, SEEK_END);
				lSize = ftell(pFile2);
				rewind(pFile2);

				fread(buffer, 1, lSize, pFile2);

				fclose(pFile2);
				FLAG_CHILD_READ = 0;
				FLAG_CHILD_WRITE = 1;
			}

			if (FLAG_CHILD_WRITE)
			{

				pFile1 = fopen("data1.txt", "a");
				if (pFile1 == NULL)
				{
					//-- Error al abrir el archivo --
					strerror_r(errno, errBuf, sizeof(errBuf));					   //-- Obtengo la leyenda del error --
					printf("Error al abrir el archivo %d: %s\r\n", errno, errBuf); //-- Imprime en stdout --
					//perror ("Error al abrir el archivo test00.txt");		//-- Imprime en stderror --
					return (-1);
				}

				fwrite(buffer, 1, lSize, pFile1);
/*
				for (i = 0; i < 100; i++)
				{
					buffer[i] = '\0';
				}
*/
				FLAG_CHILD_WRITE = 0;
				
				contador++;

				fclose(pFile1);

				printf("Hijo2 ID=%d envia señal SIGUSR1\r\n \r\n", getpid());
				kill(getppid(), SIGUSR1);
			}

			if(contador > 5)
			{
				kill(getpid(), SIGKILL);
			}

			sleep(5);
			break;

		default:
			break;
		}
	}

	printf("Muere el padre ID = %d\n\r", getpid());

	return (0);
}
