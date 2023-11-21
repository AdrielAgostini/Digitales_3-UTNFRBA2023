/* CONVCLI.C. Hecho por Dario Alpern
 *   Arranque el servidor antes que el cliente. Busque el
 *   puerto del servidor.
 *   Para ejecutar el programa escriba:
 *   convcli nombre_de_host puerto
 */
/* En primer lugar se incluyen los archivos de cabecera estandar.
 */
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <errno.h>

int main(int argc, char *argv[])
{
  int sock;
  struct sockaddr_in servAddr;
  struct hostent *hp;
  int msgLength;
#define BUFLEN 81
  char buf[BUFLEN];

  /* Los argumentos son 0: nombre del programa, 1: nombre de host y
   * 2: puerto
   */

  if (argc < 3)
  {
    printf("Ingrese convcli hostname port\n");
    exit(1);
  }

  /* 1. Crea un bloque de control de transmision. */

  if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0)
  {
    perror("No puedo obtener el socket");
    exit(1);
  }

  /* 2. Se rellena la direccion y puerto del servidor en servAddr.
   *    Primero se rellena la estructura de direccion con ceros.
   *    Despues se busca la direccion de IP de este nombre de host
   *    y se rellena con ella.
   *    Por ultimo, se pone el numero de puerto, que esta en argv[2].
   */

  memset((char *)&servAddr, 0, sizeof(servAddr));
  servAddr.sin_family = AF_INET;
  hp = gethostbyname(argv[1]);
  memcpy(&servAddr.sin_addr, hp->h_addr_list[0], hp->h_length);
  servAddr.sin_port = htons(atoi(argv[2]));

  /* 3. Se conecta al servidor. No hay que llamar a bind.
   *    El sistema asignara un puerto libre en la funcion
   *    de conexion.
   */

  if (connect(sock, (struct sockaddr *)&servAddr, sizeof(servAddr)) < 0)
  {
    perror("No se puede conectar.\n");
    exit(1);
  }

  /* 4. El cliente anuncia que ya esta listo para enviar
   *    el mensaje. Lo envia e imprime un mensaje de despedida.
   */

  do
  {
    printf("CLIENTE: ");
    fgets(buf, sizeof(buf)-1, stdin);
    if (send(sock, buf, strlen(buf)-1, 0) < 0)
    {
      if (errno == EPIPE)
      {
        printf("Conexion cortada por el servidor\n");
      }
      else
      {
        perror("Problema enviando datos.");
      }
      exit(1);
    }
    memset(buf, 0, BUFLEN);
    if ((msgLength = recv(sock, buf, BUFLEN, 0)) < 0)
    {
      if (errno == EPIPE)
      {
        printf("Conexion cortada por el servidor\n");
      }
      else
      {
        perror("Problema recibiendo datos.");
      }
      exit(1);
    }
    printf("SERVIDOR: %s\r\n", buf);
  } while (strcmp(buf, "Yo tambien. Chau."));

  close(sock);
  exit(0);
}

