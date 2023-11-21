#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <math.h>

//-- Define --
#define SizeNum		64		//-- Tamaño del numero a leer --
#define CantNumeros	20		//-- Cantidad de numeros a trabajar --


//-- Estructura ---

typedef struct punto_flotante
{
    char signo;
    double exponente;
    double fraction;
    double valor;
}punto_flotante_str;

