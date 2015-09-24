#ifndef __2048__
#define __2048__

#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <termios.h>
#include <stdbool.h>
#include <stdint.h>
#include <time.h>
#include <signal.h>
#include <stdio.h>
#include <fcntl.h>
#include "debug.h"
#include <arpa/inet.h>


#define _XOPEN_SOURCE 500
#define SIZE 4
#define SER "/tmp/ser2048.dat"

#endif

void * play(void * param);
void serialize();
int lockfile(int fd, int type);

