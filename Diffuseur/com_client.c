#include "com_client.h"

void get_address_from_caller(char * address_caller, struct sockaddr * caller, socklen_t size)
{
  if (size == sizeof(struct sockaddr_in))
  {
    char * tmp = inet_ntoa(((struct sockaddr_in *)caller)->sin_addr);
    format_adress(address_caller, tmp);
  }
  else if (size == sizeof(struct sockaddr_in6))
  {
    inet_ntop(AF_INET6, caller, address_caller, TAILLE_ADDRESS);
  }
  else
  {
    printf("WERID  IPV!!\n");
    exit(1);
  }
}

/*
   Fonction gerant l'envoi de l'historique des paquet au client
*/
void send_historique(int descSock, char* nb_mess)
{

  int set = 1;
  setsockopt(descSock, SOL_SOCKET, 13, (void *)&set, sizeof(int));

  extern Lc* listeFin;
  char * end = strrchr(nb_mess, '\r');
  if(end != NULL)
    end[0] = 0;
  int i = 0;
  int retour = 0;
  char *ptr;
  long ret;
  ret = strtol(nb_mess, &ptr, 10);
  if (ptr != nb_mess + strlen(nb_mess) || ret > 9999)
    return ;
  int nb = atoi(nb_mess);
  Lc * tmp = listeFin;
  while (i < nb && tmp != NULL)
  {
    /* sleep(1); */
    retour = send(descSock, tmp->mess, strlen(tmp->mess), 0);
    if (retour == -1)
    {
      perror(strerror(errno));
      /* exit(1); */
    }
    i++;
    tmp = tmp->prev;
  }
  retour = send(descSock, "ENDM\r\n", strlen("ENDM\r\n"), 0);
  if (retour == -1)
  {
    perror(strerror(errno));
    //exit(1);
  }
}

/*
   Fonction multithreader gerant le serveur TCP de communication Client.
*/

void * communication_client_thread(void * param_p)
{
  //sleep(1);??
  extern Process_input_client process_input_client[];
  void ** param_in = (void **) param_p;
  int descSock = *(int *)param_in[0];
  int indice_fonction_input = *(int *)param_in[1];
  char * address_caller = (char *)param_in[2];
  int recu = 0;
  int retour = 0;
  //int retour = 0;
  //char buff[5];
  char mess[TAILLE_MAX_TCP + 1];

  recu = recv(descSock, mess, TAILLE_MAX_TCP * sizeof(char), 0);
  mess[recu] = '\0';

#ifdef DEBUG
  printf("Message Recu : %s du client : %s\n", mess, address_caller);
#endif

  if (!check_terminaison_mess(mess))
  {
    retour = send(descSock, "EROR\r\n", strlen("EROR\r\n"), 0);
    //retour = send(descSock,tmp->mess,strlen(tmp->mess),0);
    if (retour == -1)
    {
      perror(strerror(errno));
      /* exit(1); */
    }

  }
  else if (strncmp(mess, "LAST", 4) == 0)
    send_historique(descSock, mess + 5);
  else if (strncmp(mess, "SUBS", 4) == 0)
    {
      add_subscriber(mess+5);
    }
  else if (strncmp(mess, "MESS", 4) == 0)
  {
    retour = send(descSock, "ACKM\r\n", strlen("ACKM\r\n"), 0);
    if(strlen(mess) == TAILLE_MAX_TCP){
      mess[13]= 0;
      mess[TAILLE_MAX_TCP-2] = 0;
      send_message(mess+14,mess + 5);
    /* retour = send(descSock,tmp->mess,strlen(tmp->mess),0); */
    if (retour == -1)
    {
      perror(strerror(errno));
      /* exit(1); */
    }
    }

  }
  else if (!process_input_client[indice_fonction_input](mess, descSock, address_caller))
  {
    retour = send(descSock, "EROR\r\n", strlen("EROR\r\n"), 0);
    /* retour = send(descSock,tmp->mess,strlen(tmp->mess),0); */
    if (retour == -1)
    {
      perror(strerror(errno));
      /* exit(1); */
    }

  }
  close(descSock);
  /* free(param_in[0]); */
  free(address_caller);
  return NULL;

}

/*
   Server TCP client
*/

void * init_client_thread(void * param_p)
{
  void ** param_in = (void **) param_p;
  char * port_diffuseur_tcp = (char *)param_in[0];
  //Variable pour les retour et erreur
  int retour = 0;
  //Socket Server IPV4
  int sockServer = socket(PF_INET, SOCK_STREAM, 0);
  struct sockaddr_in address_sock;

  address_sock.sin_family = AF_INET;
  address_sock.sin_port = htons(atoi(port_diffuseur_tcp));
  //Adresse quelqu'onque car localHost
  address_sock.sin_addr.s_addr = htonl(INADDR_ANY);

  //Fonction liant la socket a un port de la machine lui fournir un nom
  retour = bind(sockServer, (struct sockaddr *)&address_sock, sizeof(struct sockaddr_in));
  if (retour == -1)
  {
    perror(strerror(errno));
    exit(1);
  }
  //fonction lancant l'écoute par default 0 nombre de client en attente
  retour = listen(sockServer, 0);
  if (retour == -1)
  {
    perror(strerror(errno));
    exit(1);
  }
#ifdef DEBUG
  printf("Serveur TCP client creer au port: %s \n", port_diffuseur_tcp);
#endif

  while (42)
  {
    //Stocke l'addresse de l'appelant
    struct sockaddr caller;
    //Doit être init sinon bug
    socklen_t size = sizeof(caller);
    int * descSock = malloc(sizeof(int));

    if (descSock == NULL)
    {
      exit(1);
    }

    *descSock = accept(sockServer, &caller, &size);
    char * address_caller = malloc(sizeof(char) * (TAILLE_ADDRESS + 1));
    get_address_from_caller(address_caller, &caller, size);
    if (retour == -1)
    {
      perror(strerror(errno));
      continue;
    }

#ifdef DEBUG
    printf("Connexion accepter avec : %s\n", address_caller);
#endif

    /*
     *Creation de thread
     *via pthread
     *Memoire partagé
     *
     */
    pthread_t th;
    void * param_out[3];
    param_out[0] = (void *)descSock;
    param_out[1] = param_in[1];
    param_out[2] = (void *)address_caller;
    retour = pthread_create(&th, NULL, communication_client_thread, param_out);
    if (retour != 0)
    {
      perror(strerror(errno));
      close(*descSock);
      free(descSock);
      continue;
    }
    //pthread_join(th,NULL);

  }
  close(sockServer);
  return 0;
}
