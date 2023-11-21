/* CONVSERV.C: Servidor de conversacion hecho por Dario Alpern */

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
#include <errno.h>

void childWork(int sockClient);
int sendToClient(int sockClient, char *str, char *ipAddr, int Port);

/**********************************************************/
/* funcion MAIN                                           */
/* Orden Parametros: Ninguno                              */
/*                                                        */
/**********************************************************/
int main(int argc, char *argv[])
{
  socklen_t length;
  int pid;
  struct sockaddr_in servAddr;
  int sockMain, sockClient;

  /* 1. Crea el bloque de control de transmision maestro.
   */

  if ((sockMain = socket(PF_INET, SOCK_STREAM, 0)) < 0)
  {
    perror("El servidor no puede abrir el socket.");
    exit(1);
  }

  /* 2. Crea una estructura de datos para mantener la direccion
   *    y puerto local de IP a usar. Se podrian aceptar conexiones
   *    de clientes a cualquier direccion local de IP (INADDR_ANY).
   *    Como el servidor no usa ningun puerto publico, se fija en
   *    puerto = 0. La llamada a bind asignara un puerto al
   *    servidor y lo escribira en el TCB.
   */

  memset( (char *) &servAddr, 0, sizeof(servAddr));
  servAddr.sin_family = AF_INET;
  servAddr.sin_addr.s_addr = htonl(INADDR_ANY);
  if (argc == 1)
  {
    servAddr.sin_port = 0;  /* El sistema operativo brinda el puerto */
  }
  else
  {    
    servAddr.sin_port = htons(atoi(argv[1]));
  }

  /* 3. Llama a bind. Bind recoge numero de puerto y lo
   *    escribe en el TCB.
   */

  if (bind(sockMain, (struct sockaddr *)&servAddr, sizeof(servAddr)))
  {
    perror("bind");
    exit(1);
  }

  /* Queremos ver el numero del puerto. Se usara la funcion
   * getsockname() para copiar el puerto en servAddr.
   */

  length = sizeof(servAddr);
  if (getsockname(sockMain, (struct sockaddr *)&servAddr, &length))
  {
    perror("getsockname()");
    exit(1);
  }
  printf("SERVIDOR: Escuchando por el puerto %d\n", ntohs(servAddr.sin_port));

  /* 4. Configura una cola que pueda dar cabida a cinco clientes.
   */

  listen(sockMain, 5);

  /* 5. Espera a que llegue un cliente. Accept devolvera
   *    un nuevo descriptor de conector que se usara con dicho cliente.
   */

  for (;;)
  {
    if ((sockClient = accept(sockMain, 0, 0)) < 0)
    {
      perror("accept");
      exit(1);
    }

  /* 6. Crea un proceso hijo para manejar al cliente.
   */

    if ((pid = fork()) < 0)
    {
      printf("fork");
      exit(1);
    }
    if (pid == 0)
    {                  // Proceso Hijo
      childWork(sockClient);
      close(sockClient);
      exit(0);
    }
    close(sockClient);
  }
}

 /* El hijo lee un buffer de entrada, imprime un mensaje y termina.
  */

#define BUFLEN 81
void childWork(int sockClient)
{
  char buf[BUFLEN];
  int msgLength;
  char ipAddr[20];
  socklen_t length;
  struct sockaddr_in clientAddr;
  int Port;

  /* 8. Obtener direccion IP y puerto del cliente
   */
  
  length = sizeof(clientAddr);
  if (getpeername(sockClient, (struct sockaddr *)&clientAddr, &length))
  {
    perror("peername");
    exit(1);
  }

  strcpy(ipAddr, inet_ntoa(clientAddr.sin_addr));
  Port = ntohs(clientAddr.sin_port);

  /* 9. Primera parte de la conversacion.
   */

  while (1)
  {
    memset(buf, 0, BUFLEN);
    if ((msgLength = recv(sockClient, buf, BUFLEN, 0)) < 0)
    {
      if (errno == EPIPE)
      {
        printf("Conexion cortada por el cliente %s:%d\n", ipAddr, Port);
        return;
      }
      perror("Fallo en la recepcion de datos.");
      exit(1);
    }
    printf("CLIENTE %s:%d : %s\r\n",ipAddr, Port, buf);
    if (strcmp(buf, "Hola") == 0)
    {
      break;
    }
    sendToClient(sockClient, "Tenes que escribir 'Hola'", ipAddr, Port);
  }
  sendToClient(sockClient, "Que tal?", ipAddr, Port);

  /* 10. Segunda parte de la conversacion.
   */

  while (1)
  {
    memset(buf, 0, BUFLEN);
    if ((msgLength = recv(sockClient, buf, BUFLEN, 0)) < 0)
    {
      if (errno == EPIPE)
      {
        printf("Conexion cortada por el cliente %s:%d\n", ipAddr, Port);
        return;
      }
      perror("Fallo en la recepcion de datos.");
      exit(1);
    }
    printf("CLIENTE %s:%d : %s\r\n",ipAddr, Port, buf);
    if (strcmp(buf, "Bien") == 0)
    {
      break;
    }
    sendToClient(sockClient, "Tenes que escribir 'Bien'", ipAddr, Port);
  }
  sendToClient(sockClient, "Yo tambien. Chau.", ipAddr, Port);
}

int sendToClient(int sockClient, char *str, char *ipAddr, int Port)
{
  printf("SERVIDOR: %s\r\n", str);
  if (send(sockClient, str, strlen(str), 0) < 0)
  {
    if (errno == EPIPE)
    {
      printf("Conexion cortada por el cliente %s:%d\n", ipAddr, Port);
    }
    else
    {
      perror("Fallo en la transmision de datos.");
    }
    exit(1);
  }
  return 0;
}

