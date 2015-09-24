#ifndef __com_diff__
#define __com_diff__

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


#define TAILLE_ADDRESS 15
#define TAILLE_PORT 4
#define TAILLE_ID 8
#define REGI 4
#define NBR_DIFF 5

#endif

void register_diff(Dlist*, int, char*);
void ask_RUOK(Dlist*, int, struct node*);
void format_regi(char * regi_message, char * reponse);
void format_port(char * new_port, const char * old_port);
void format_id(char * new_id, const char * old_id);
void format_adress(char * new_adress, const char * old_adress);
