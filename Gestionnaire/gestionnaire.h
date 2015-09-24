#ifndef __gestionnaire__
#define __gestionnaire__

#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <errno.h>
#include <unistd.h>
#include <string.h>
#include <sys/socket.h>
#include <netdb.h>
#include <pthread.h>
#include "listDc.h"
#include <fcntl.h>
#include <signal.h>
#include <poll.h>
#include "com_client.h"
#include "com_diff.h"
void * lancer_gestionnaire (void *);
void * print_state (void * par);
#endif
