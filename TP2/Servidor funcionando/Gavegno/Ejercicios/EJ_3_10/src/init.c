
#include "../inc/init.h"
#include "../inc/handler.h"

void init_signal(void)
{
    signal(SIGCHLD, SIGCHLD_handler);
	signal(SIGTERM, SIGTERM_handler);
	signal(SIGUSR1, SIGUSR1_handler);
	signal(SIGUSR2, SIGUSR2_handler);
	signal(SIGINT, SIGINT_handler);
	//signal(SIGKILL, SIGKILL_handler);
}

