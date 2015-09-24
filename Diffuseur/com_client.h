#ifndef __com_client__
#define __com_client__

#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <errno.h>
#include <unistd.h>
#include <err.h>
#include <string.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netdb.h>
#include <pthread.h>
#include "radio.h"
#include "format_mess.h"
#include "debug.h"
#include "signal.h"
#include "email.h"

#endif

void * init_client_thread(void * param_p);
