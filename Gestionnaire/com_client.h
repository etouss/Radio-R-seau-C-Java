#ifndef __com_client__
#define __com_client__

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
#include "fcntl.h"
#include <signal.h>
#include <poll.h>

#endif

void lister(Dlist*, int);
void format_num_diff (char*, int);