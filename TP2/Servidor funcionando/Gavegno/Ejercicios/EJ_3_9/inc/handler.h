
#include "define.h"


extern int pid;
extern int var, varP;
extern int cantchld;
extern int idchld[];
extern char exitwhile;
extern int FLAG_KILL_PADRE;
extern int FLAG_CLOSE;

void SIGCHLD_handler(int sig);

void SIGTERM_handler(int sig);

void SIGUSR1_handler(int sig);
void SIGUSR2_handler(int sig);
void SIGINT_handler(int sig);
void SIGKILL_handler(int sig);