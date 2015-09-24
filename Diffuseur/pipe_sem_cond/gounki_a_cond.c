
#include "gounki_a.h"


/* int desc_pipe_write_a = 0; */
/* int desc_pipe_read_a = 0; */
char coup_tcp[13];
char coup_part[13];
char rdy = 0;
pthread_mutex_t synchro_mutex;
pthread_cond_t synchro_cv;

char * send_plateau_a()
{
  #ifdef DEBUG
  write_tty("Demande de plateau : \n");
  #endif

  static int player = 0;
  static int first = 1;
  static pthread_t th;
  /* char buffer[13]; */
  int retour = 0;
  if (first)
  {
    #ifdef DEBUG
    write_tty("Init des lock et cond \n");
    #endif
    first = 0;
    coup_part[0] = 0;
    coup_tcp[0] = 0;
    pthread_mutex_init(&synchro_mutex, NULL);
    pthread_cond_init (&synchro_cv, NULL);
    /* void ** rdy_p = malloc(sizeof(void *) * 2); */
    /* rdy_p[0] = &desc_pipe_write_a; */
    /* rdy_p[1] = &desc_pipe_read_a; */
    #ifdef DEBUG
    write_tty("Creation du Gounki \n");
    #endif

    retour = pthread_create(&th, NULL, jouer_radio_VS, NULL /* (void *)(rdy_p) */);

    if (retour != 0)
    {
      perror(strerror(errno));
      exit(1);
    }
    /* GERER LE PREMIER WAIT */
    /* while (desc_pipe_read_a == 0 || desc_pipe_write_a == 0) {} */
    #ifdef DEBUG
    write_tty("Attente de Gounki prêt \n");
    #endif

    pthread_mutex_lock(&synchro_mutex);
    while (!rdy)
      pthread_cond_wait(&synchro_cv, &synchro_mutex);
    /* coup_part[0] = 0; */
    rdy = 0;
    pthread_mutex_unlock(&synchro_mutex);

    #ifdef DEBUG
    write_tty("Gounki prêt \n");
    #endif


    return string_plat();
    /* while (42) */
    /*   if (desc_pipe_write_ia)return string_plat(); */
  }
  if (player)
  { 
      
    pthread_mutex_lock(&synchro_mutex);
    while (coup_tcp[0] == 0){
      #ifdef DEBUG
      write_tty("Attente du coup de l'autre Radio \n");
      #endif
      pthread_cond_wait(&synchro_cv, &synchro_mutex);
    }
    #ifdef DEBUG
    write_tty("Coup de l'autre Radio %s\n",coup_tcp);
    #endif
    strcpy(coup_part, coup_tcp);
    coup_tcp[0]=0;
    pthread_cond_signal(&synchro_cv);
    rdy = 0;
    pthread_mutex_unlock(&synchro_mutex);

    /* write(desc_pipe_write_a, coup_adv, strlen(coup_adv) - 1); */
    /* coup_adv[0] = 0; */
    /* buffer[read(desc_pipe_read_a, buffer, 12)] = 0; //SYNCHRO */
  }
  else
  {
    pthread_mutex_lock(&synchro_mutex);
    send_coup_a();
    #ifdef DEBUG
    write_tty("Notre coup : %s \n",coup_part);
    #endif
    pthread_cond_signal(&synchro_cv);
    rdy = 0;
    pthread_mutex_unlock(&synchro_mutex); //SYNCHRO
  }
  #ifdef DEBUG
  write_tty("Attente de MAJ Gounki \n");
  #endif
  pthread_mutex_lock(&synchro_mutex);
  while (!rdy)
    pthread_cond_wait(&synchro_cv, &synchro_mutex);
  send_msg_to_diff(coup_part);
  #ifdef DEBUG
  write_tty("Coup réelement joué : %s \n",coup_part);
  #endif  
  rdy = 0;
  coup_part[0] = 0;
  pthread_mutex_unlock(&synchro_mutex);
  player = !player;
  return string_plat();


}


void send_coup_a()
{
  char * coup = get_best();
  /* printf("COUP === %s\n", coup); */
  /* fflush(stdout); */
  /* write(desc_pipe_write_a, coup, strlen(coup)); */
  strcpy(coup_part, coup);
  raz_vote();
}

int process_input_client_gounki_a(char * buff, int descSock, char * address_caller)
{
  char  mess[TAILLE_MAX_TCP];
  if (strncmp(buff, "INFO", 4) == 0)
  {
    format_mess_desc(mess, "Un serveur de jeu Gounki communautaire ou vous affronter une IA !");
    send(descSock, mess, strlen(mess), 0);
    send(descSock, "VOTE\r\n", strlen("VOTE\r\n"), 0);
    send(descSock, "ENDM\r\n", strlen("ENDM\r\n"), 0);
    return 1;
  }
  else if (strncmp(buff, "HELP", 4) == 0)
  {
    /* printf("%lu\n", strlen("Pour faire un déplacment : c2-d3\nPour faire un déploiement simple : c2+b2 pour les carrés\nPour faire un déploiement simple : c2*b3 pour les rond\nPour les deploiement composées c2*b3-b4 pour ce commencé par rond\nPour les deploiement composées c2+c3-b4 pour ce commencé par rond"));           */
    if (strncmp(buff + 5, "VOTE", 4) == 0)
    {
      format_mess_help(mess, "Comande pour participer au jeu VOTE suivit du coup : VOTE a1-b1", "VOTE");
      send(descSock, mess, strlen(mess), 0);
      return 1;
    }
  }
  else if (strncmp(buff, "VOTE", 4) == 0)
  {
    /* char * end = strchr(buff, '\r'); */
    char * end = strchr(buff, '\n');
    end[0] = 0;
    receive_vote(buff + 5);
    return 1;

  }
  return 0;
}

int process_input_diffu_gounki_a(char * buf)
{
  if (strncmp(buf, "PLAY", 4) == 0)
  {
    pthread_mutex_lock(&synchro_mutex);
    strcpy(coup_tcp,buf+5); 
    #ifdef DEBUG
    write_tty("Coup recu via TCP: %s \n",coup_tcp);
    #endif
    pthread_cond_broadcast(&synchro_cv);
    pthread_mutex_unlock(&synchro_mutex);
    return 1;
  }
  return 0;
}

