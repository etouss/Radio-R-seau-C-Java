
#include "gounki_a.h"


int desc_pipe_write_a = 0;
int desc_pipe_read_a = 0;
char coup_adv[13];
sem_t *  sem_tcp;

char * send_plateau_a()
{
  static int player = 0;
  static int first = 1;
  static pthread_t th;
  char buffer[13];
  int retour = 0;
  if (first)
  {
    first = 0;
    coup_adv[0] = 0;
    if ((sem_tcp = sem_open("/tmp/semaphore_tcp", O_CREAT | O_TRUNC, 0644, 0)) == SEM_FAILED )
    {
      perror(strerror(errno));
      exit(1);
    }
    void ** rdy_p = malloc(sizeof(void *) * 2);
    rdy_p[0] = &desc_pipe_write_a;
    rdy_p[1] = &desc_pipe_read_a;
    retour = pthread_create(&th, NULL, jouer_radio_VS, (void *)(rdy_p));

    if (retour != 0)
    {
      perror(strerror(errno));
      exit(1);
    }
    while (desc_pipe_read_a == 0 || desc_pipe_write_a == 0) {}
    return string_plat();
    /* while (42) */
    /*   if (desc_pipe_write_ia)return string_plat(); */
  }
  if (player)
  {
    buffer[read(desc_pipe_read_a, buffer, 12)] = 0;
    if (strncmp(buffer, "ready", 5) == 0)
    {
      printf("HERE WAIT BEGIN: %lu \n ", strlen(coup_adv));
      fflush(stdout);
      if (strlen(coup_adv) == 0)
      {
        retour = sem_wait(sem_tcp);
        if (retour != 0)
        {
          perror(strerror(errno));
          exit(1);
        }
      }
      printf("HERE WAIT END: %s\n", coup_adv);
      fflush(stdout);
      /* A CHANFER LE -1 */
      write(desc_pipe_write_a, coup_adv, strlen(coup_adv) - 1);
      coup_adv[0] = 0;
    }
    buffer[read(desc_pipe_read_a, buffer, 12)] = 0; //SYNCHRO
  }
  else
  {
    buffer[read(desc_pipe_read_a, buffer, 12)] = 0;
    if (strncmp(buffer, "ready", 5) == 0)
      send_coup_a();
    buffer[read(desc_pipe_read_a, buffer, 12)] = 0; //SYNCHRO
  }
  /* printf("RETOUR%s\n",buffer); */
  player = !player;
  return string_plat();


}


void send_coup_a()
{
  char * coup = get_best();
  /* printf("COUP === %s\n", coup); */
  /* fflush(stdout); */
  write(desc_pipe_write_a, coup, strlen(coup));
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
    char * end = strchr(buff, '\r');
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
    int i = 0;
    for (i = 0; i < strlen(buf + 5); i++)
      coup_adv[i] = buf[5 + i];
    coup_adv[i] = 0;
    printf("HERE NOTY OK: %s\n", coup_adv);
    sem_post(sem_tcp);
    return 1;
  }
  return 0;
}

