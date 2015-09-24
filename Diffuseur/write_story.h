
#ifndef __write_story__
#define __write_story__

#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <errno.h>
#include <unistd.h>
#include <err.h>
#include <string.h>
#include <netdb.h>
#include <fcntl.h>
#include <time.h>
#include "debug.h"
#include "radio.h"

#endif

char * write_story();
int process_input_client_write_story(char * mess_client,int sock_client,char * address_caller);

