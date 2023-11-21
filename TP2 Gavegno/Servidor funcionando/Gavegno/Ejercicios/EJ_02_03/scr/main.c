#include "../inc/funciones.h"

//-- MAIN --
int main (int argc, char *argv[]) 
{
char *pMem; 
FILE *pFile;							//--Puntero al archivo --
int cant_numeros;
long lSize;
double promedio = 0, varianza = 0;
double var[CantNumeros];

	if(argc > 1)  
	{  
    	lSize = abrir_archivo(&pFile, argv[1]); 
		if(lSize == -1)
		{
			return(lSize);
		}
	}  

	if(pedir_memoria(&pMem, lSize) < 0)
	{
		return -2;
	}

	leer_datos( &pMem, &pFile, lSize);

	cant_numeros = procesar_datos( &pMem, var, lSize);

	//-- calculo del promedio --
	promedio = calcular_promedio( var,cant_numeros);

	//-- calculo de la varianza --
	varianza = calcular_varianza( var, promedio, cant_numeros);

	//-- Escribo en la terminal --
	printf("cantidad de numeros= %d\n\r", cant_numeros);
	printf("Promedio: %f \n\r", promedio);
	printf("Varianza: %f \n\r", varianza);

	cerrar_archivo(&pFile);
	Liberar_memoria(&pMem);

	return(0);
} 

