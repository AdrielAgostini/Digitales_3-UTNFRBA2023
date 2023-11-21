#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <errno.h>
#include <string.h>
#include <sys/wait.h>
#include <sys/types.h>

#include <sys/stat.h>
#include <fcntl.h>


#define cantVar     2
#define MAXCHLD    2
