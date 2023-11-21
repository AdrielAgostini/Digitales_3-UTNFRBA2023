#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>

char buffer[2000];

int main(int argc, char *argv[])
{
  int fd = open ("/dev/acelerometro-td3", O_RDWR);
  if (fd < 0)  
  {
    printf("El device driver no está instalado.\n");
    return 2;
  }
  printf("%s\n", "Se abrio el driver");

  close(fd);
  return 0;	
}