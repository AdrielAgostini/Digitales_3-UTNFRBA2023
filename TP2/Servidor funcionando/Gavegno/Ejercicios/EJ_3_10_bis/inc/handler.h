
#include "define.h"

extern int FLAG_KILL_PADRE;

void SIGCHLD_handler(int sig);

void SIGTERM_handler(int sig);

void SIGUSR1_handler(int sig);
void SIGUSR2_handler(int sig);
void SIGINT_handler(int sig);
void SIGKILL_handler(int sig);