
#include "../inc/define.h"

char abrir_archivo(FILE **pFile, char *file_name);
char cerrar_archivo(FILE **pFile);
char pedir_memoria(char **pMem, long Size);
void Liberar_memoria(char **mem);

void leer_datos(char **pMem, FILE **pFile, long lSize);

int procesar_datos(char **pMem, double *ptr, long Size);

float calcular_promedio(double *ptr, int Size);
float calcular_varianza(double *ptr, float prom, int Size);
