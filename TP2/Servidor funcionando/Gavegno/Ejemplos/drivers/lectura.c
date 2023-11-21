// Hecho por Dario Alpern
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
char buffer[2000];
int main(int argc, char *argv[])
{
  int bytesRead, fd;
  if (argc != 2)
  { 
    printf("Formato linea de comandos: lectura numero\n");
    return 1;
  }
  fd = open ("/dev/letras", O_RDWR);
  if (fd < 0)  
  {
    printf("El device driver no está instalado.\n");
    return 2;
  }
  bytesRead = read(fd, buffer, atoi(argv[1]));
  buffer[bytesRead] = 0;
  printf("%s\n", buffer);
  return 0;	
}
