#ifndef __com_gestionnaire__
#define __com_gestionnaire__

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
#include "format_mess.h"
#include "config_radio.h"
#include "debug.h"
#include <poll.h>

#define DEBUT_PORT 1000

#endif

int * init_radio (const char * addresse_gestionnaire, const char * port_gestionnaire , const char * port_diffuseur_tcp ,
                  const char * addresse_diffuseur_multi, const char * port_diffuseur_multi, const char * nom);
void * communication_radio_thread(void * desc_sock_p);
void send_msg_to_diff(char * mess);

