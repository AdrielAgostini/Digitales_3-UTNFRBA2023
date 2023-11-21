

#include "../inc/funciones.h"


/*----------------------Funciones---------------------------------*/

/*
* Abrir archivo pasado como parametro
* Parametro:
    **pFile: puntero al archivo
    *file_name: puntero que contiene el nombre del archivo
* Retorno:
    tamaño del archivo, o -1 error al abrir el archivo
*/
char abrir_archivo(FILE **pFile, char *file_name)
{
char errBuf[256];
char retorno = -1;

	//-- Abro el archivo como lectura --
	*pFile = fopen(file_name, "r");
	if(*pFile == NULL)
	{
		//-- Error al abrir el archivo --
		strerror_r(errno, errBuf, sizeof(errBuf));				//-- Obtengo la leyenda del error --
		printf("Error al abrir el archivo %d: %s\r\n", errno, errBuf);	//-- Imprime en stdout --
		//perror ("Error al abrir el archivo test00.txt");		//-- Imprime en stderror --
		return retorno;
	}

    //obtener el tamanio del archivo
  	fseek(*pFile , 0 , SEEK_END);
  	retorno = ftell(*pFile);
  	rewind(*pFile);

    return retorno;
}

/*
* Cierro el archivo
* Parametros:
    **pFile: puntero al archivo
* Retorno:
    -2 error al cerrar archivo
*/
char cerrar_archivo(FILE **pFile)
{
char rVal;
char errBuf[256];

	//-- Cierro el archivo --
	rVal = fclose(*pFile);
	if(rVal != 0) 
    {
		//-- Error al cerrar el archivo --
		strerror_r(errno, errBuf, sizeof (errBuf));					//-- Obtengo la leyenda del error --
		printf("Error al cerrar el archivo %d: %s\r\n", errno, errBuf); //-- Imprime en stdout --
		//perror ("Error al cerrar el archivo test00.txt");			//-- Imprime en stderror --
		return(-2);
	}
    return 0;
}

/*
* Pido memoria
* Parametros:
    **pMem: puntero a la memoria a pedir
    Size: cantidad de memoria a pedir
* Retorno:
    -1 error al pedir memoria
*/
char pedir_memoria(char **pMem, long Size)
{
  	//asignar memoria para contener todo el archivo
  	*pMem = (char*)malloc(sizeof(char)*Size);
  	if(*pMem == NULL) 
	{
		fputs ("Error de memoria",stderr); 
		exit(2);
        return -1;
	}
    return 0;
}

/*
* Libero el espacio de memoria 
* Parametros:
    **mem: puntero a memoria
*/
void Liberar_memoria(char **mem)
{
    free(*mem);
    return;
}

/*
* Lee los datos que contiene un archivo y los guarda en memoria
* Paraetros:
    **pMem: puntero a memoria
    ** pFile: puntero al archivo
    lSize: tamaño del archivo
* Retorna: 
    void
*/
void leer_datos(char **pMem, FILE **pFile, long lSize)
{
long cant_leida;

	cant_leida = fread(*pMem, 1, lSize, *pFile);
	if(cant_leida != lSize) 
	{
		fputs("Error de lectura",stderr); 
		exit(3);
	}
    return;
}

/*
* Esta funcion levanta la informacion de la memoria para procesarla y guardarla

* Parametros: 
    **pMem: puntera al lugar de memoria a leer
    *ptr: puntero al vector donde se va a almacenar los datos
    Size: tañano de la memoria a leer
* Retorno:
    retorna la cantidad de numeros que se puderon leer de la memoria
*/
int procesar_datos(char **pMem, double *ptr, long Size)
{
long cant_leida = 0;
char aux = 0;
int indice = 0, div = 10;
char flag = 0;
    
    while (cant_leida <= Size )
    {
        aux = *(*pMem + cant_leida);
        cant_leida++;
        if (aux == ',')
        {
            //termino el numero
            flag = 0;
            div = 10;
            if(indice < CantNumeros-1)
            {
                indice++;
            }
        }
        if (aux == '.')
        {
            //comienza la parte decimal
            flag = 1;
        }
        if(aux >= '0' && aux <= '9')
        {
            switch (flag)
            {
            case 0:     //parte entera
                ptr[indice] = ptr[indice]*10 + (aux-48);
                break;
            
            case 1:     //parte decimal
                ptr[indice] += (float)(aux-48)/div;
                div = div*10;
                break;
            
            default:
                flag = 0;
                break;
            }
        }
    }
    return indice;
}


/*
* Calcula el promedio de los valores pasados por parametro
* Parametros: 
    *ptr: puntero al vector que contiene los numeros a promediar
    Size: cantidad de numeros a promediar
* Retorno:
    Retorna el valor del promedio
*/
float calcular_promedio(double *ptr, int Size)
{
int i;
float retorno = 0;

	if((Size > 0) && (ptr != NULL))
	{
		for( i = 0; i < Size; i++)
		{
			retorno = ptr[i] + retorno;
		}
		retorno = retorno/Size;
	}

	return retorno;
}


/*
* Calcula la varianza de los valores pasados por parametro
* Parametros: 
    *ptr: puntero al vector que contiene los numeros
    Size: cantidad de numeros
* Retorno:
    Retorna el valor de la varianza
*/
float calcular_varianza(double *ptr, float prom, int Size)
{
int i;
float retorno = 0;

	if((Size > 0) && (ptr != NULL))
	{
		for( i = 0; i < Size; i++)
		{
			retorno = (ptr[i] - prom) + retorno;
		}
		retorno = retorno/Size;
	}
	return retorno;
}

