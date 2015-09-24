#include "gestionnaire.h"

Dlist *p_list;
int verbose = 1;

void * lancer_gestionnaire (void *descSockP){
  int descSock = *(int *) descSockP;
  int retour;
  char reponse[1024];
  int recu = recv (descSock, reponse, 1023 * sizeof (char), 0);
  reponse[recu] = '\0';
  if(verbose){
  printf ("%s\n", reponse);
  }
  if (strncmp (reponse, "LIST", 4) == 0){
    lister(p_list, descSock);
  }
  else if (strncmp (reponse, "REGI", 4) == 0){
    register_diff(p_list, descSock, reponse);
  }
  else{
    retour =  send (descSock, "ERRO", strlen ("ERRO") * sizeof (char), 0);
    if (retour == -1){
      perror (strerror (errno));
      exit (1);
    }
  }
  close(descSock);
  free(descSockP);
  return NULL;
}
void * print_state (void * args){
  while (42){
    sleep(10);
    parcours_liste(p_list);
  }
  return NULL;
}


int main (int argc, char const *argv[]){
  if (argc == 2){
    verbose = 0;
  }
  else if (argc == 3){
    if (strcmp(argv[1],"-v")!= 0){
    printf("Usage: gestionnaire [-v] <PORT>\n");
    exit(1);
    }
    verbose = 1;
  }
  else{
    printf("Usage: gestionnaire [-v] <PORT>\n");
    exit(1);
  }
  //initalisation de la liste
  p_list = dlist_new();
  //Variable pour les retour et erreur
  int retour = 0;
  int d = 0;
  //Socket Server IPV4
  int sockServer = socket(PF_INET, SOCK_STREAM, 0);
  struct sockaddr_in address_sock;
  address_sock.sin_family = AF_INET;
  address_sock.sin_port = htons(atoi (argv[verbose+1]));
  //Adresse que quiconque car localHost
  address_sock.sin_addr.s_addr = htonl(INADDR_ANY);
  //Fonction liant la socket a un port de la machine lui fournir un nom
  retour =  bind (sockServer, (struct sockaddr *) &address_sock, sizeof(struct sockaddr_in));
  if (retour == -1){
    perror (strerror (errno));
    exit (1);
  }
  //fonction lancant l'écoute par default 0 nombre de client en attente
  retour = listen(sockServer, 0);
  if (retour == -1){
    perror (strerror (errno));
    exit (1);
  }
  printf("Lancement du gestionnaire...\n");
  if(verbose){
    pthread_t deBug;
    d = pthread_create(&deBug, NULL, print_state, NULL);
    if (d != 0){
      perror(strerror(errno));
    }
  }
  while (42){
    //Stocke l'addresse de l'appelant
    struct sockaddr_in caller;
    //Doit être init sinon bug
    socklen_t size = sizeof (caller);
    int *descSock = malloc (sizeof(int));
    if (descSock == NULL){
      exit (1);
    }
    *descSock = accept (sockServer, (struct sockaddr *) &caller, &size);
    if (retour == -1){
      perror(strerror (errno));
      continue;
    }
    pthread_t th;
    retour = pthread_create(&th, NULL, lancer_gestionnaire, descSock);
    if (retour != 0){
      perror(strerror(errno));
      close(*descSock);
      free(descSock);
      continue;
    }
  }
  close (sockServer);
  return 0;
}
