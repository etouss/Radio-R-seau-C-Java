
#ifndef __read_story__
#define __read_story__

#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <errno.h>
#include <unistd.h>
#include <err.h>
#include <string.h>
#include <netdb.h>
#include <fcntl.h>
#include "radio.h"
#include "debug.h"

#endif

char * read_story();
int process_input_client_read_story(char * mess_client,int sock_client,char * address_caller);

