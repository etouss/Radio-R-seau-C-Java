
#ifndef __gounki_a__
#define __gounki_a__

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
#include <semaphore.h>
#include "debug.h"
#include "com_gestionnaire.h"
#endif

char * send_plateau_a();
void send_coup_a();
int process_input_client_gounki_a(char * mess_client,int sock_client,char * address_caller);
int process_input_diffu_gounki_a(char * buf);
