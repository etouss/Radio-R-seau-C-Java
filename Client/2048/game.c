#include "game.h"

int main(int argc, char const *argv[])
{
  //Variable pour les retour et erreur
  int retour = 0;
  //Socket IPV4
  //
  pthread_t th_radio;
  retour = pthread_create(&th_radio,NULL,play,NULL);

  struct sockaddr_in adress_sock;

  //Si Ip connu:
  struct in_addr addrIPV4;
  retour = inet_aton(argv[1], &addrIPV4);
  if (retour == 0)
  {
    printf("Adresse IP non valide\n");
    exit(1);
  }
  adress_sock.sin_family = AF_INET;
  //BigENDIAN
  adress_sock.sin_port = htons(atoi(argv[2]));
  adress_sock.sin_addr = addrIPV4;

  while (42)
  {
    sleep(10);
    serialize();
    int desc_sock = socket(PF_INET, SOCK_STREAM, 0);
    retour = connect(desc_sock, (struct sockaddr *)&adress_sock, sizeof(struct sockaddr_in));
    if (retour == -1)
    {
      perror(strerror(errno));
      exit(1);
    }
    int rc = 0;
    char mess[1025];
    int fic = open(SER, O_RDONLY);
    lockfile(fic, F_RDLCK);
    while ((rc = read(fic, mess, 1024)) == 1024)
    {
      write_tty("send\n");
      mess[rc] = 0;
      retour = send(desc_sock, mess, strlen(mess), 0);
      if (retour == -1)
      {
        perror(strerror(errno));
        exit(1);
      }
    }
    mess[rc] = 0;
    retour = send(desc_sock, mess, strlen(mess), 0);
    if (retour == -1)
    {
      perror(strerror(errno));
      exit(1);
    }
    //Les free
    write_tty("send over\n");
    close(fic);
    close(desc_sock);
  }

  return 0;
}

/*Historiquement / Obsolete:
  struct hostent * host = gethostbyname("lucien.informatique.univ-paris-diderot.fr");
  if(host==NULL){
  printf("Hostname inconnue\n");
  exit(1);
  }
  struct in_addr **addresses=(struct in_addr**)host->h_addr_list;
  if(*addresses!=NULL)addrIPV4 = **addresses;
  else{
  printf("adresse IP non trouvé\n");
  exit(1);
  }
  */
