
#ifndef __format_mess__
#define __format_mess__

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "debug.h"



#define TAILLE_PAQUET 161
#define TAILLE_MESSAGE 140
#define TAILLE_HISTORIQUE 9999
#define TAILLE_ID 8
#define TAILLE_NUM_MESS 4

#define TAILLE_ADDRESS 15
#define TAILLE_PORT 4

#define TAILLE_NUM_MESS 4

#define TAILLE_MAX_TCP 156

#define TAILLE_CMD 4


#endif

void format_regi(char * mess,const char * nom,const char * addresse_diffuseur_multi,const char * port_diffuseur_multi,char * adresse_diffuseur_tcp ,const char *port_diffuseur_tcp);
void format_mess_udp(char * paquet,char * mess,const char * id,int num);
void format_mess_desc(char * mess, char * desc);
void format_mess_help(char * mess, char *desc_cmd , char * cmd);
void format_adress(char *,const char*);
int check_terminaison_mess(char * mess);
