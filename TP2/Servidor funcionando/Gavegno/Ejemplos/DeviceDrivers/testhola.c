// Hewcho por Dario Alpern

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>

int main (void){
	int fd;
	char nada[10];

	fd=open ("/dev/hola",O_RDWR);
	if (fd<=1) {
		printf("No se pudo abrir el dispositivo /dev/hola\n");
		return -1;
	}else
		printf("/dev/hola abierto OK!\n");

	sleep(1);
	printf("Escribiendo en /dev/hola...\n");
	write(fd,"a",1);
	sleep(1);
	printf("Leyendo de /dev/hola...\n");
	read(fd,nada,1);
	sleep(1);
	printf("Cerrando /dev/hola...\n");
	close (fd);
	return 0;
}
