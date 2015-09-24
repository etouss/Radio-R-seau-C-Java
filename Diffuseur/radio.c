#include "radio.h"


/*
   Verrou pour gerer l'envoi de message
*/
pthread_mutex_t verrou= PTHREAD_MUTEX_INITIALIZER;

int sock;
struct sockaddr *  adress_sock;
const char * id;

/*
  Gestion de la liste Doublement chainé des messages
*/

Lc* listeDebut;
Lc* listeFin;
void createElementLc(char * mess, Lc * l)
{
    int i = 4;
    l->mess[0] = 'O';
    l->mess[1] = 'L';
    l->mess[2] = 'D';
    l->mess[3] = 'M';
    for(i=4; i<strlen(mess); i++)
        {
            l->mess[i] = mess[i];
        }
    l->mess[i] = 0;
    l->next = NULL;
    l->prev = NULL;
}

void deleteLc()
{
    Lc * tmp = listeDebut;
    listeDebut = listeDebut->next;
    listeDebut -> prev = NULL;
    free(tmp);
}


void addLc(Lc * l)
{
    static int nbElement = 0;
    if(nbElement == 0)
        {
            listeDebut = l;
            listeFin = l;
            nbElement++;
            return;
        }
    else if(nbElement >= TAILLE_HISTORIQUE) deleteLc();
    l->prev = listeFin;
    listeFin->next = l;
    listeFin = l;
    nbElement++;
}

void print_list()
{
    int nb = 0;
    Lc * tmp = listeDebut;
    while(tmp != 0 && tmp != NULL)
        {
            #ifdef DEBUGLIST
            printf("%s",tmp->mess);
            #endif
            tmp = tmp->next;
            nb++;
        }
    printf("Nombre d'elements : %d \n",nb);
  }

/*
   Fonction gerant l'envoi des message de facon recursive si le message > 140 octects
*/

/*
   To free local variables faster !!!
*/

void sub_send_mess_rec(char * mess,int num_mess,const char * id_sender)
{
    char mess_to_send[TAILLE_PAQUET+1];
    int retour = 0;
    /* printf("%lu\n",strlen(mess)); */
    /* fflush(stdout); */
    format_mess_udp(mess_to_send,mess,id_sender,num_mess);
    struct Lc *l = malloc(sizeof(Lc));
    createElementLc(mess_to_send,l);
    addLc(l);
    retour = sendto(sock,mess_to_send,TAILLE_PAQUET,0,adress_sock,(socklen_t)sizeof(struct sockaddr_in));
    if(retour == -1)
        {
            perror(strerror(errno));
            exit(1);
        }
}

void send_message_rec(char * mess, const char * id_sender)
{
    static int num_mess = 0;
    if(num_mess == 10000)num_mess = 0;
    sub_send_mess_rec(mess,num_mess,id_sender);
    num_mess ++;
    if(strlen(mess)>TAILLE_MESSAGE)send_message_rec(mess+TAILLE_MESSAGE,id_sender);
}

/*
  Fonction encapsulant la fonction recursive pour eviter les deadLock
*/
void send_message(char *mess, const char * id_sender)
{
    pthread_mutex_lock(&verrou);
    send_message_rec(mess,id_sender);
    pthread_mutex_unlock(&verrou);
}


/*
   Fonction gerant la creation du multicaste
*/
void run_radio(const char * addresse_diffuseur_multi, const char * port_diffuseur_multi,const char * nom,int indice_fonction_output)
{

     #ifdef DEBUG
     printf("Init du MultiCast à l'adresse %s sur le port %s \n",addresse_diffuseur_multi,port_diffuseur_multi);
     #endif

    //socket envoi UDP
    sock=socket(PF_INET,SOCK_DGRAM,0);
    id = nom;
    //BroadCast
    int retour = 0;
    //Get adress server
    adress_sock = NULL;
    struct addrinfo *first_info;
    struct addrinfo hints;
    memset(&hints, 0, sizeof(struct addrinfo));
    hints.ai_family = AF_INET;
    hints.ai_socktype=SOCK_DGRAM;
    retour = getaddrinfo(addresse_diffuseur_multi,port_diffuseur_multi,NULL,&first_info);
    if(retour !=0)
        {
            printf("%s\n",gai_strerror(retour));
            exit(1);
        }
    if(first_info!=NULL)
        {
            adress_sock = (struct sockaddr *)first_info->ai_addr;
        }

     #ifdef DEBUG
     printf("Creation du serveur multicast réussi \n");
     #endif


    /* char tampon[161]; */
    /* //char entier[4]; */
    /* //int i=0; */
    /* char * nom_file = "text.txt"; */
    /* int file = 0; */
    /* int rc = 0; */

    extern Process_output_multicast process_output_multicast[];
    extern int multicaste_delay[];
    /* if((file = open(nom_file,O_RDONLY))==-1) errx(1,"impossible d ouvrir l archive %s \n",nom_file); */
    while(42)
        {

            /* rc = read(file,tampon,160); */
            /* tampon[rc]=0; */
            char * tampon = process_output_multicast[indice_fonction_output]();
            send_message(tampon,id);
            #ifdef DEBUG
            printf("Etat de l'historique def DEBUGLIST for detail \n");
            print_list();
            #endif
            /* printf("%s\n",tampon); */
            /* fflush(stdout); */
            free(tampon);
            sleep(multicaste_delay[indice_fonction_output]);
        }

    close(sock);
}
