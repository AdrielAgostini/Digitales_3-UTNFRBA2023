#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <math.h>

struct confServer {
    int conexiones_max;         // en configuracion.cfg = a
    int backlog;                // en configuracion.cfg = b
    int muestreo;               // en configuracion.cfg = m
};

int leerConfServer(void);
/*
*   Leo el archivo de configuracion
*   retorna < 0 en caso de error
*/
int leerConfServer(void)
{
    int retorno = 0, i;
    char *pMem;
    FILE *pFile; //--Puntero al archivo --
    long lSize, cant_leida;
    char errBuf[256];
    char rVal;

    //-- Abro el archivo como lectura --
    pFile = fopen("Configuracion.cfg", "r");
    if (pFile == NULL)
    {
        //-- Error al abrir el archivo --
        strerror_r(errno, errBuf, sizeof(errBuf));                     //-- Obtengo la leyenda del error --
        printf("Error al abrir el archivo %d: %s\r\n", errno, errBuf); //-- Imprime en stdout --
        //perror ("Error al abrir el archivo test00.txt");		//-- Imprime en stderror --
        return -1;
    }

    //obtener el tamanio del archivo
    fseek(pFile, 0, SEEK_END);
    lSize = ftell(pFile);
    rewind(pFile);

    //asignar memoria para contener todo el archivo
    pMem = (char *)malloc(sizeof(char) * lSize);
    if (pMem == NULL)
    {
        fputs("Error de memoria", stderr);
        return -2;
    }

    cant_leida = fread(pMem, 1, lSize, pFile);
    if (cant_leida != lSize)
    {
        fputs("Error de lectura", stderr);
        return -1;
    }

    /*********************************************
 ***** PROCESAR DATOS DE CONFIGURACION  *****
**********************************************/
    char Parametro;
    char buffer[10];
    int BUFFindex = 0;
    struct confServer auxconfServer;

    for (i = 0; i < lSize; i++)
    {

        switch (pMem[i])
        {
        case 'b':
            Parametro = 'b';
            break;
        case 'c':
            Parametro = 'c';
            break;
        case 'm':
            Parametro = 'm';
            break;
        case '=':
            break;
        case '\n':
            switch (Parametro)
            {
            case 'b':
                auxconfServer.backlog = atoi(buffer);
                BUFFindex = 0;
                break;
            case 'c':
                auxconfServer.conexiones_max = atoi(buffer);
                BUFFindex = 0;
                break;
            case 'm':
                auxconfServer.muestreo = atoi(buffer);
                BUFFindex = 0;
                break;
            }
            break;

        default:
            buffer[BUFFindex] = pMem[i];
            BUFFindex++;
            break;
        }
    }
/*
    //otro semaforo configID
    semop(semaforoConfig, &tomar, 1); //Tomo el semaforo
    memcpy(configuracionServer, &auxconfServer, sizeof(struct confServer));
    printf("|*******************************************************\n");
    printf("|                                                       \n");
    printf("|Cantidad maxima de conexiones =    %d                  \n", configuracionServer->conexiones_max);
    printf("|Backlog =                          %d                  \n", configuracionServer->backlog);
    printf("|Muestreo del filtro =              %d                  \n", configuracionServer->muestreo);
    printf("|                                                       \n");
    printf("|*******************************************************\n");
    semop(semaforoConfig, &liberar, 1); //Libero el semaforo
*/
    //-- Cierro el archivo --
    rVal = fclose(pFile);
    if (rVal != 0)
    {
        //-- Error al cerrar el archivo --
        strerror_r(errno, errBuf, sizeof(errBuf));                      //-- Obtengo la leyenda del error --
        printf("Error al cerrar el archivo %d: %s\r\n", errno, errBuf); //-- Imprime en stdout --
        return (-2);
    }

    free(pMem);

    return retorno;
}


    /*
    if (leerConfServer() < 0)
    {   //no se encontro el archivo o no se pudo leer
        semop(semaforoConfig, &tomar, 1); //Tomo el semaforo
        configuracionServer->backlog = 20;
        configuracionServer->conexiones_max = 100;
        configuracionServer->muestreo = 2;
        semop(semaforoConfig, &liberar, 1); //Libreo el semaforo
    }
*/