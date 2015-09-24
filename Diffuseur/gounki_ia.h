#ifndef __gounki_ia__
#define __gounki_ia__

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

#include "Gounki/partieHeuristique.h"
#include "Gounki/Jouer.h"
#include "Gounki/modeTest.h"
#include "Gounki/test.h"
#include <pthread.h>
#include <errno.h>

#include "format_mess.h"
#include "vote_lib.h"
#include "debug.h"
#endif

char * send_plateau_ia();
void send_coup_ia();
int process_input_client_gounki_ia(char * mess_client,int sock_client,char * address_caller);
