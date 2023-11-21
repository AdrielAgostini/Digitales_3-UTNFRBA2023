
//#include "../inc/define.h"
#include "../inc/handler.h"
#include "../inc/init.h"

#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/msg.h>
#include <sys/ipc.h>

#include <sys/ipc.h>
#include <sys/sem.h>
//-------Variables Externas--------------

//----- Variables Globales -----

//----- FLAGS ------------

//----- struct -----------

typedef struct mymsgbuf
{
	long mtype;
	int buff;
} mess_t;

union semun
{
	int val;			   /* Value for SETVAL */
	struct semid_ds *buf;  /* Buffer for IPC_STAT, IPC_SET */
	unsigned short *array; /* Array for GETALL, SETALL */
	struct seminfo *__buf; /* Buffer for IPC_INFO (Linux specific) */
};

//-- MAIN --
int main(int argc, char *argv[])
{
	int pid = 1, pid2 = 1;
	int contador = 0, first = 0;
	int process = 0;
	char exitwhile = 1;

	mess_t mensaje;
	key_t mykey;
	int qid;

	int length;

	length = sizeof(mess_t) - sizeof(long);

	init_signal();

	mykey = ftok(".", 'm');
	qid = msgget(mykey, IPC_CREAT | 0660);

	pid = fork();
	if (pid == 0)
	{
		//codigo del hijo1
		process = 1;
		printf("Proceso hijo ID=%d, Nº de proceso %d\r\n", getpid(), process);
	}
	else if (pid > 0)
	{
		//Código del padre
		pid2 = fork();
		if (pid2 == 0)
		{
			//codigo del hijo2
			process = 2;
			printf("Proceso hijo ID=%d, Nº de  proceso %d\r\n", getpid(), process);
		}
	}

	while (exitwhile)
	{
		switch (process)
		{
		case 0:
			//codigo del padre
			if (FLAG_KILL_PADRE == 1)
			{
				exitwhile = 0;
				kill(pid, SIGKILL);
				kill(pid2, SIGKILL);
			}
			sleep(5);
			break;

		case 1:
			//codigo del hijo1
			sleep(1);
			if (first == 0)
			{
				mensaje.buff = contador;
				mensaje.mtype = 1;
				msgsnd(qid, &mensaje, length, 0);
				first = 1;
				printf("Hijo1 %d envio: %d \r\n \r\n", getpid(), mensaje.buff);
				sleep(1);
			}
			
			msgrcv(qid, &mensaje, length, 1, 0);
			printf("Hijo1 %d recibio: %d\r\n", getpid(), mensaje.buff);
			sleep(1);
			mensaje.buff++;
			msgsnd(qid, &mensaje, length, 0);
			printf("Hijo1 %d envio: %d \r\n \r\n", getpid(), mensaje.buff);

			contador++;
			if (contador > 10)
			{
				kill(getpid(), SIGKILL);
			}
			break;

		case 2:
			//codigo del hijo2
			sleep(1);
			msgrcv(qid, &mensaje, length, 1, 0);
			printf("Hijo2 %d recibio: %d\r\n", getpid(), mensaje.buff);
			sleep(1);
			msgsnd(qid, &mensaje, length, 0);
			printf("Hijo2 %d envio: %d \r\n \r\n", getpid(), mensaje.buff);

			contador++;
			if (contador > 5)
			{
				kill(getpid(), SIGKILL);
			}
			break;

		default:
			break;
		}
	}

	msgctl(qid, IPC_RMID, 0); // Destruir la cola de mensajes
	printf("Muere el padre ID = %d\n\r", getpid());

	return (0);
}
