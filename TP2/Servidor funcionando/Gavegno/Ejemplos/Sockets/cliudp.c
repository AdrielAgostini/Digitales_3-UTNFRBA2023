#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

/**********************************************************/
/* funcion MAIN                                           */
/* Orden Parametros: IP destino, puerto , mensaje         */
/*                                                        */
/**********************************************************/
int main(int argc, char *argv[])
{
  int s;
  struct sockaddr_in bs,des;
  char resp[255];
  socklen_t sd;

  if (argc == 4)
  {
    // Creamos el socket
    s = socket(AF_INET, SOCK_DGRAM, 0);   // Socket UDP.
    if (s != -1)
    {
      bs.sin_family = AF_INET;
      bs.sin_port = htons(0); //Obtiene cualquier puerto disponible en la maquina
      bs.sin_addr.s_addr = htonl(INADDR_ANY); //Obtiene cualquier IP de la maquina

      //Asigna un nombre local al socket
      if( bind(s,(struct sockaddr*)&bs, sizeof(bs)) != -1)
      {    // Completar la estructura des con informacion del server.
        des.sin_family = AF_INET; 
        des.sin_addr.s_addr = inet_addr(argv[1]);
        des.sin_port = htons(atoi(argv[2]));

          // Envia el string
        sendto(s,argv[3],strlen(argv[3])+1,0,(struct sockaddr*)&des,sizeof(des));
        printf("\n\n->Enviando: %s, a: %s en el puerto: %s \n",argv[3], argv[1], argv[2]);
          // Recibe el string del servidor
          // recvfrom rellena des con informacion del servidor y
          // escribe en sd la longitud de la estructura.
        sd = sizeof(des);
        recvfrom(s,resp, sizeof(resp), 0, (struct sockaddr*)&des, &sd);
        printf("<-Recibido: %s\n",resp);
          // Cierra el socket
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
    printf("\n\nFormato linea de comandos: cliudp IP destino, puerto, mensaje\n\n");
  }
  return 0;
}
 
