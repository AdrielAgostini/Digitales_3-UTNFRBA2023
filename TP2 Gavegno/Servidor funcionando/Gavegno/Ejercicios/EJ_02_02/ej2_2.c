#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>

//-- Define --
//#define FILE_NAME 		"ej01.c"    	//!< Nombre del archivo.

int main ( int argc, char *argv[]) 
{
char* file_name; 
FILE *pFile;				//--Puntero al archivo --
char *buffer;				//--Puntero a la memoria asignada --
char rVal, i;
char errBuf[256];
int cant_leida;
long lSize;

   if(argc > 1)  
   {  
       file_name = argv[1];  
   }  
   else  
   {  
       file_name = "FileNew.txt";  
   }  

	//-- Abro el archivo como lectura --
	pFile = fopen (file_name, "r");
	if (pFile == NULL) {
		//-- Error al abrir el archivo --
		strerror_r (errno, errBuf, sizeof (errBuf));			//-- Obtengo la leyenda del error --
		printf ("Error al abrir el archivo %d: %s\r\n", errno, errBuf);	//-- Imprime en stdout --
		//perror ("Error al abrir el archivo test00.txt");		//-- Imprime en stderror --
		return (-1);
	}

	//obtener el tamaño del archivo
  	fseek (pFile , 0 , SEEK_END);
  	lSize = ftell (pFile);
  	rewind(pFile);

  	//asignar memoria para contener todo el archivo
  	buffer = (char*)malloc(sizeof(char)*lSize);
  	if(buffer == NULL) 
	{
		fputs ("Error de memoria",stderr); 
		exit (2);
	}

	cant_leida = fread(buffer, 1, lSize, pFile);
	if(cant_leida != lSize) 
	{
		fputs("Error de lectura",stderr); 
		exit (3);
	}

	//-- Escribo en la terminal --
	write(1, buffer, cant_leida);  

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
	
	free (buffer);

	return (0);
} 
