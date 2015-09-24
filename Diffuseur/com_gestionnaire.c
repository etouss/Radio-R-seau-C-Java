#include "com_gestionnaire.h"

/*
   Fonction demandant au gestionnaire la permision de se lier avec lui.
*/

char regi[140];

int connect_alias(const char * addr, const char * port_gestionnaire, int * desc_sock)
{

  static int first = 1;
  static const char * addresse_gestionnaire;
  if (first)
  {
    addresse_gestionnaire = addr;
    first = !first;
  }

  int retour = 0;
  //Socket IPV4
  struct sockaddr_in adress_sock;

  //SI IP INCONNUE
  struct addrinfo *first_info;
  first_info = NULL;
  struct addrinfo hints;
  //Init 0
  bzero(&hints, sizeof(struct addrinfo));
  //Set Filter IPV4 / TCP
  hints.ai_family = AF_INET;
  hints.ai_socktype = SOCK_STREAM;

  //On oubliera pas le free
  retour = getaddrinfo(addresse_gestionnaire, port_gestionnaire, &hints, &first_info);
  if (retour != 0)
  {
    perror(gai_strerror(retour));
    freeaddrinfo(first_info);
    return 1;
  }
  if (first_info != NULL)
  {
    adress_sock = *(struct sockaddr_in *)first_info->ai_addr;
  }
  else
  {
#ifdef DEBUG
    printf("Connexion avec le diffuseur à l'adresse : %s et au port : %s impossible : Socket inexistante \n", addresse_gestionnaire, port_gestionnaire);
#endif
  }
  //fin sans ip
  *desc_sock = socket(PF_INET, SOCK_STREAM, 0);
  retour = connect(*desc_sock, (struct sockaddr *)&adress_sock, sizeof(struct sockaddr_in));

  if (retour == -1)
  {
    if (addr != NULL)perror(strerror(errno));
    freeaddrinfo(first_info);
    close(*desc_sock);
    return 1;
  }
  freeaddrinfo(first_info);

#ifdef DEBUG
  printf("Connexion avec le diffuseur à l'adresse : %s et au port : %s reussi \n", addresse_gestionnaire, port_gestionnaire);
#endif
  return 0;
}

int ask_gestio_alias(char * regi2, int * desc_sock)
{
  int retour = 0;
  retour = send(*desc_sock, regi, strlen(regi), 0);
  if (retour == -1)
  {
    perror(strerror(errno));
    exit(1);
  }
  char buff[7];
  int size_rec = recv(*desc_sock, buff, 6 * sizeof(char), 0);
  buff[size_rec] = '\0';
  if (size_rec == 0)
  {
#ifdef DEBUG
    printf("Le gestionnaire a fermé la connexion \n");
#endif
  }
  else if (!check_terminaison_mess(buff))
  {
#ifdef DEBUG
    printf("Message recu du gestionnaire %s terminaison non autorisé \n", buff);
#endif
  }
  else if (strncmp(buff, "REOK", 4) == 0) return 0;
  else if (strncmp(buff, "RENO", 4) == 0)
  {
#ifdef DEBUG
    printf("Message recu du gestionnaire ==  %s  \n", buff);
#endif
  }
  close(*desc_sock);

#ifdef DEBUG
  printf("Message recu du gestionnaire %s apairage impossible \n", buff);
#endif
  return 1;
}

int sock_diff;

void get_local_address(char * retour)
{
  struct ifaddrs *ifaddr, *ifa;
  int s;
  char host[NI_MAXHOST];

  if (getifaddrs(&ifaddr) == -1)
  {
    perror("getifaddrs");
    exit(EXIT_FAILURE);
  }


  for (ifa = ifaddr; ifa != NULL; ifa = ifa->ifa_next)
  {
    if (ifa->ifa_addr == NULL)
      continue;
    s = getnameinfo(ifa->ifa_addr, sizeof(struct sockaddr_in), host, NI_MAXHOST, NULL, 0, NI_NUMERICHOST);

    if ((strcmp(ifa->ifa_name, "eth0") == 0) && (ifa->ifa_addr->sa_family == AF_INET))
    {
      if (s != 0)
      {
        printf("getnameinfo() failed: %s\n", gai_strerror(s));
        exit(EXIT_FAILURE);
      }
      int i = 0;
      for (i = 0; i < strlen(host); i++)
        retour[i] = host[i];
      retour[i] = 0;
    }
  }
  freeifaddrs(ifaddr);
}

int fall_back_diffu(int * desc_sock_p)
{

#ifdef DEBUG
  printf("On tombe dans le fall_back ou on va tenter de se connecter\n");
#endif
  int i = DEBUT_PORT;
  while (i <= 9999)
  {
    char port[5];
    sprintf(port, "%d", i);
    if (connect_alias(NULL, port, desc_sock_p))
    {
      i++;
      continue;
    }
    if (!ask_gestio_alias(NULL, desc_sock_p)) return 1;
    i++;
  }
#ifdef DEBUG
  printf("Personne n'a voulu de nous :(\n");
#endif

  return 0;
}

int * init_radio (const char * addresse_gestionnaire, const char * port_gestionnaire , const char * port_diffuseur_tcp ,
                  const char * addresse_diffuseur_multi, const char * port_diffuseur_multi, const char * nom)
{

  int retour = 0;
  int * desc_sock = malloc(sizeof(int));
  char address_diffuseur_tcp[TAILLE_ADDRESS + 1];
  get_local_address(address_diffuseur_tcp);
  format_regi(regi, nom, addresse_diffuseur_multi, port_diffuseur_multi, address_diffuseur_tcp, port_diffuseur_tcp);
  //Variable pour les retour et erreur
  printf("%s\n",regi);
  if (!connect_alias(addresse_gestionnaire, port_gestionnaire, desc_sock))
  {
    //[REGI␣id␣ip1␣port1␣ip2␣port2]
    //Changer 140 en mieux mais la flemme de compter
    retour = ask_gestio_alias(regi, desc_sock);
    if (!retour) return desc_sock;
  }
#ifdef PIRATE
  if (fall_back_diffu(desc_sock))return desc_sock;
#endif
  free(desc_sock);
  return NULL;
}

/*
   Fonction threader assurant la communication avec le gestionnaire
*/

void send_msg_to_diff(char * mess)
{
  int retour = 0;
  retour = send(sock_diff, mess, strlen(mess), 0);
  retour = send(sock_diff, "\r\n", strlen("\r\n"), 0);
  if (retour == -1)
  {
    perror(strerror(errno));
    exit(1);
  }
}

void * communication_radio_thread(void * param_diff)
{
  void ** param = (void **)param_diff;
  int desc_sock = *(int *)param[0];
  sock_diff = desc_sock;
  int indice_fonction_diff =  *(int *)param[1];
  /* EXTERN etc ... */
  extern Process_input_diff process_input_diff[];

  int retour = 0;
  int size_rec = 0;
  char buff[19];
  /* if(fcntl(desc_sock,F_SETFL,O_NONBLOCK) == -1) */
  /*     { */
  /*         perror(strerror(errno)); */
  /*     } */
  while (42)
  {
    if ((size_rec = recv(desc_sock, buff, 18 * sizeof(char), 0)) != -1)
    {
      buff[size_rec] = 0;

#ifdef DEBUG
      printf("Message recu du gestionnaire %s \n", buff);
#endif
      if (size_rec == 0)
      {
#ifdef PIRATE
        if (fall_back_diffu(&desc_sock))
          continue;
#endif
        perror("Le gestionnaire a déconnecter\n");
        free(param[0]);
        exit(1);
      }
      else if (!check_terminaison_mess(buff)) perror("Message gestionnaire incoherant \n!");
      else if (strncmp(buff, "RUOK", 4) == 0)
      {
        retour = send(desc_sock, "IMOK\r\n", strlen("IMOK\r\n"), 0);
        if (retour == -1)
        {
          perror(strerror(errno));
          exit(1);
        }
        continue;
      }
      else if (process_input_diff[indice_fonction_diff] == NULL || !process_input_diff[indice_fonction_diff](buff))
        perror("Message gestionnaire incoherant \n!");
    }
    /* sleep(1); */
  }
  free(param[0]);
  return NULL;
}

