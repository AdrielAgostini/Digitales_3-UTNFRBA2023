#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
/**********************************************************/
/* función MAIN */
/* Orden Parametros: Puerto */
/* */
/**********************************************************/
int main(int argc, char *argv[])
{
  int s;
  struct sockaddr_in bs, in;
  char entrada[255];
  socklen_t sd;
  char ipAddr[20];
  int Port;

  if (argc == 2) 
  { 
    // Creamos el socket
    s = socket(AF_INET,SOCK_DGRAM,0);   // SOCK_DGRAM para UDP
    if (s != -1)
    {
      bs.sin_family = AF_INET;
      bs.sin_port = htons(atoi(argv[1])); //Asigna el puerto especificado por la línea de comandos
      bs.sin_addr.s_addr = htonl(INADDR_ANY); //IP cualquiera de la máquina

      //Asigna un nombre local al socket
      if( bind(s,(struct sockaddr*)&bs, sizeof(bs)) != -1)
      {
        printf("\n\aServidor ACTIVO escuchando en el puerto: %s\n",argv[1]);
        //El while permite atender a multiples clientes
        while (1)
        {
          // Recibe la cadena del cliente
          // recvfrom rellena la estructura in con informacion del cliente
          // y pone en sd la longitud de la estructura.
          sd = sizeof(in);
          if ( recvfrom(s,entrada, sizeof(entrada), 0, (struct sockaddr*) &in, &sd)== -1)
          perror("Error en recvfrom");
          
          strcpy(ipAddr, inet_ntoa(in.sin_addr));
          Port = ntohs(in.sin_port);
          printf("CLIENTE %s:%d : %s\r\n",ipAddr, Port, entrada);
  
          // Devuelve la cadena al cliente
          // sendto usa la esturctura in para saber a quien mandarle la info.
          if (sendto(s,strcat(entrada,"\0"),strlen(entrada)+1,0,(struct sockaddr*) &in, sizeof(in)) == -1)
          perror("Error en sendto");
        }
        //Se cierra el socket
        close(s);
      }
      else
      {
        printf("ERROR al nombrar el socket\n");
      }
    }
    else
    {
      printf("ERROR: El socket no se ha creado correctamente!\n");
    }
  }
  else
  {
    printf("\n\nFormato de linea de comandos: servudp Puerto\n\n");
  }
  return 0;
}