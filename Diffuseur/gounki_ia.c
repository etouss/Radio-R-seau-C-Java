#include "gounki_ia.h"


extern char coup_part[13];
extern char rdy;
extern pthread_mutex_t synchro_mutex;
extern pthread_cond_t synchro_cv;


char * send_plateau_ia()
{
  static int first = 1;
  static pthread_t th;
  int retour = 0;
  if (first)
  {
#ifdef DEBUG
    printf("Init des lock et cond \n");
#endif
    first = 0;
    coup_part[0] = 0;
    pthread_mutex_init(&synchro_mutex, NULL);
    pthread_cond_init (&synchro_cv, NULL);
    /* void ** rdy_p = malloc(sizeof(void *) * 2); */
    /* rdy_p[0] = &desc_pipe_write_a; */
    /* rdy_p[1] = &desc_pipe_read_a; */
#ifdef DEBUG
    printf("Creation du Gounki \n");
#endif

    retour = pthread_create(&th, NULL, jouer_radio_IA, NULL /* (void *)(rdy_p) */);

    if (retour != 0)
    {
      perror(strerror(errno));
      exit(1);
    }
    /* GERER LE PREMIER WAIT */
    /* while (desc_pipe_read_a == 0 || desc_pipe_write_a == 0) {} */
#ifdef DEBUG
    printf("Attente de Gounki prêt \n");
#endif

    pthread_mutex_lock(&synchro_mutex);
    while (!rdy)
      pthread_cond_wait(&synchro_cv, &synchro_mutex);
    /* coup_part[0] = 0; */
    rdy = 0;
    pthread_mutex_unlock(&synchro_mutex);

#ifdef DEBUG
    printf("Gounki prêt \n");
#endif


    return string_plat();
  }
  pthread_mutex_lock(&synchro_mutex);
  send_coup_ia();
#ifdef DEBUG
  printf("Notre coup : %s \n", coup_part);
#endif
  pthread_cond_signal(&synchro_cv);
  rdy = 0;
  pthread_mutex_unlock(&synchro_mutex); //SYNCHRO

#ifdef DEBUG
  printf("Attente de MAJ Gounki \n");
#endif
  pthread_mutex_lock(&synchro_mutex);
  while (!rdy)
    pthread_cond_wait(&synchro_cv, &synchro_mutex);
#ifdef DEBUG
  printf("Coup réelement joué : %s \n", coup_part);
#endif
  rdy = 0;
  coup_part[0] = 0;
  pthread_mutex_unlock(&synchro_mutex);

  return string_plat();
}


void send_coup_ia()
{
  char * coup = get_best();
  /* printf("COUP === %s\n", coup); */
  /* fflush(stdout); */
  /* write(desc_pipe_write_a, coup, strlen(coup)); */
  strcpy(coup_part, coup);
  raz_vote();
}

int process_input_client_gounki_ia(char * buff, int descSock, char * address_caller)
{
  int set = 1;
  setsockopt(descSock, SOL_SOCKET, 13, (void *)&set, sizeof(int));

  int retour = 0;
  char  mess[TAILLE_MAX_TCP];
  if (strncmp(buff, "INFO", 4) == 0)
  {
    format_mess_desc(mess, "Un serveur de jeu Gounki communautaire ou vous affronter une IA !");
    send(descSock, mess, strlen(mess), 0);
    send(descSock, "VOTE\r\n", strlen("VOTE\r\n"), 0);
    retour = send(descSock, "ENDM\r\n", strlen("ENDM\r\n"), 0);
    if (retour == -1)
    {
      perror(strerror(errno));
      /* exit(1); */
    }
    return 1;
  }
  else if (strncmp(buff, "HELP", 4) == 0)
  {
    /* printf("%lu\n", strlen("Pour faire un déplacment : c2-d3\nPour faire un déploiement simple : c2+b2 pour les carrés\nPour faire un déploiement simple : c2*b3 pour les rond\nPour les deploiement composées c2*b3-b4 pour ce commencé par rond\nPour les deploiement composées c2+c3-b4 pour ce commencé par rond"));           */
    if (strncmp(buff + 5, "VOTE", 4) == 0)
    {
      format_mess_help(mess, "Comande pour participer au jeu VOTE suivit du coup : VOTE a1-b1", "VOTE");
      retour = send(descSock, mess, strlen(mess), 0);
      if (retour == -1)
      {
        perror(strerror(errno));
        /* exit(1); */
      }
      return 1;
    }
  }
  else if (strncmp(buff, "VOTE", 4) == 0)
  {
    //char * end = strchr(buff, '\r');
    //end[0] = 0;
    receive_vote(buff + 5);
    return 1;

  }
  return 0;
}

