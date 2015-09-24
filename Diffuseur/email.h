
#ifndef __email__
#define __email__

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
#include <fcntl.h>
#include <ifaddrs.h>
#include "debug.h"
#include <poll.h>

#endif

void add_subscriber(char * email);
void * warn_subscriber(void * null);
