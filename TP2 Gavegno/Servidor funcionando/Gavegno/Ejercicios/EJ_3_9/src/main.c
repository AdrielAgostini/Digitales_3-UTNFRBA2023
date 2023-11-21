
//#include "../inc/define.h"
#include "../inc/handler.h"
#include "../inc/init.h"

//-------Variables Externas--------------


//----- Variables Globales -----
char exitwhile = 1;
//----- FLAGS ------------
int FLAG_FIRST = 1;
int FLAG_CLOSE = 0;

//-- MAIN --
int main (int argc, char *argv[]) 
{
int i = 0;
char file_name[10]; 
char errBuf[256];
char exitwhile = 1;
FILE *pFile;				//--Puntero al archivo --
char rVal;

	init_signal();
    
	while(exitwhile)
	{
		if(pid == 0)
    	{
			//Código del hijo
			//Creo archivo
			sprintf(file_name, "%d", getpid());
			//-- Abro el archivo como lectura --
			pFile = fopen ( file_name, "w+");
			if (pFile == NULL) 
			{
				//-- Error al abrir el archivo --
				strerror_r (errno, errBuf, sizeof (errBuf));			//-- Obtengo la leyenda del error --
				printf ("Error al abrir el archivo %d: %s\n\r", errno, errBuf);	//-- Imprime en stdout --
				//perror ("Error al abrir el archivo test00.txt");		//-- Imprime en stderror --
				return (-1);
			}

			//fseek (pFile , 0 , SEEK_END);

			//escrivo archivo	
			fprintf(pFile,"Proceso hijo ID=%d, mi padre es ID=%d, mi grupo ID=%d, var=%d\n\r", getpid(), getppid(), getpgrp(),var);
			//printf("Proceso hijo ID=%d, mi padre es ID=%d, mi grupo ID=%d, var=%d\r\n", getpid(), getppid(), getpgrp(),var);

			//Código del hijo
			//-- Cierro el archivo --
			rVal = fclose(pFile);
			if(rVal != 0) 
 			{
				//-- Error al cerrar el archivo --
				strerror_r(errno, errBuf, sizeof (errBuf));			//-- Obtengo la leyenda del error --
				printf("Error al cerrar el archivo %d: %s\r\n", errno, errBuf); //-- Imprime en stdout --
				//perror ("Error al cerrar el archivo test00.txt");		//-- Imprime en stderror --
				return(-2);
			}

			sleep(10);
    	}
		else if(pid > 0)
		{
			//Código del padre
			printf("Yo soy tu padre %d, mi grupo ID=%d\n\r", getpid(), getpgrp());

			if(FLAG_KILL_PADRE == 1)
			{
				for(i=0; i < cantchld; i++)
				{
					kill(idchld[i], SIGKILL);
				}
				exitwhile = 0;
			}

			sleep(10);
		}
	}

	
	printf("Muere el padre ID = %d\n\r", getpid());

	return(0);
} 

