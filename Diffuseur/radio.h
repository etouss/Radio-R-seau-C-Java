#ifndef __radio__
#define __radio__

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
#include <pthread.h>
#include "config_radio.h"
#include "format_mess.h"
#include "debug.h"

typedef struct Lc
{
    char mess[TAILLE_PAQUET +1];
    struct Lc * prev;
    struct Lc * next;
} Lc;


#endif

void run_radio(const char * addresse_diffuseur_multi, const char * port_diffuseur_multi,const char * nom,int indice_fonction_output);
void send_message(char *mess,const char * id_sender);
