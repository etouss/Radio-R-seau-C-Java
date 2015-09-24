
#ifndef __guess_number__
#define __guess_number__

#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <errno.h>
#include <unistd.h>
#include <err.h>
#include <string.h>
#include <fcntl.h>
#include <string.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netdb.h>
#include <time.h>

#include <pthread.h>
#include <errno.h>
#include <signal.h>

#include "format_mess.h"
#include "debug.h"
#include "radio.h"
#endif

char * send_recap_guess();
int process_input_client_guess_number(char * mess_client,int sock_client,char * address_caller);
