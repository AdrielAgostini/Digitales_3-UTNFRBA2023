#include<unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main (int argc, char *argv[])
{
  int fd;
  char string_send[200];
  char string_rcv[200];

  if (argc != 2)
  {
    printf("Formato: test string_a_convertir_a_mayúsculas\n");
    return 1;
  }
  fd = open ("/dev/toupper", O_RDWR);
  if (fd < 0)
  {
    perror("No puedo abrir toupper");
    exit(1);
  }
  strncpy(string_send, argv[1], sizeof(string_send));
  write (fd, string_send, strlen(string_send));
  read (fd, string_rcv, sizeof(string_rcv));
  printf("Conversión: %s\n", string_rcv);
  return 0;
}
