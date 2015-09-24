
#include "gounki_a.h"


/* int desc_pipe_write_a = 0; */
/* int desc_pipe_read_a = 0; */
char coup_adv[13];
char coup_jouer[13];
sem_t *  mutex_play_a;
sem_t *  mutex_tcp_a;


/* typedef struct vote */
/* { */
/*     char coup[13]; */
/*     int nb_vote; */
/*     struct vote * next; */
/* } Vote; */
/*  */
/* Vote* vote_debut; */
/* Vote* vote_fin; */
/*  */
/* void createElementLcV(char * coup, Vote * v) */
/* { */
/*     int i = 0; */
/*     for(i=0; i<strlen(coup)&&i<12; i++) */
/*         { */
/*             v->coup[i] = coup[i]; */
/*             v->nb_vote =1; */
/*         } */
/*     v->coup[12]=0; */
/*     v->next = NULL; */
/* } */
/*  */
/* void addLcV(Vote * v) */
/* { */
/*     if(vote_debut == 0) */
/*         { */
/*             vote_debut = v; */
/*             vote_fin = v; */
/*             return; */
/*         } */
/*     vote_fin->next = v; */
/*     vote_fin = v; */
/* } */
/*  */
/* void raz_vote() */
/* { */
/*   Vote * tmp = vote_debut; */
/*   while(tmp != NULL && tmp != 0){ */
/*     Vote *next = tmp->next; */
/*     free(tmp); */
/*     tmp = next; */
/*   } */
/*   vote_fin = 0; */
/*   vote_debut = 0; */
/* } */

char * send_plateau_a()
{
  static int first = 1;
  static pthread_t th;
  /* char buffer[13]; */
  int retour = 0;
  if (first)
  {
    first = 0;
    coup_adv[0] = 0;
    if ((mutex_tcp_a = sem_open("/tmp/mutex_tcp_a", O_CREAT | O_TRUNC, 0644, 0)) == SEM_FAILED )
    {
      perror(strerror(errno));
      exit(1);
    }
    if ((mutex_play_a = sem_open("/tmp/mutex_play_a", O_CREAT | O_TRUNC, 0644, 0)) == SEM_FAILED )
    {
      perror(strerror(errno));
      exit(1);
    }

    retour = pthread_create(&th, NULL, jouer_radio_VS, NULL);

    if (retour != 0)
    {
      perror(strerror(errno));
      exit(1);
    }
    /* while (desc_pipe_read_a == 0 || desc_pipe_write_a == 0) {} */
    /* sleep(1); */
    sem_wait(mutex_play_a);
    return string_plat();
    /* while (42) */
    /*   if (desc_pipe_write_ia)return string_plat(); */
  }

  printf("LOCK1\n");
  fflush(stdout);
  sem_wait(mutex_play_a);
  if (strlen(coup_adv) == 0)sem_wait(mutex_tcp_a);
  sem_post(mutex_play_a);
  printf("LOCK2\n");
  fflush(stdout);
  sem_wait(mutex_play_a);
  printf("Coup Vraiment joué A %s \n", coup_jouer);
  fflush(stdout);
  printf("LOCK3\n");
  fflush(stdout);
  sem_wait(mutex_play_a);
  send_coup_a();
  printf("LOCK4\n");
  fflush(stdout);
  sem_wait(mutex_play_a);
  printf("Coup Vraiment joué B %s \n", coup_jouer);



  /* buffer[read(desc_pipe_read_a, buffer, 12)] = 0; */
  /* if (strncmp(buffer, "ready", 5) == 0) */
  /* { */
  /*   printf("HERE WAIT BEGIN: %lu \n ",strlen(coup_adv)); */
  /*   fflush(stdout); */
  /*   if (strlen(coup_adv) == 0) */
  /*   { */
  /*     retour = sem_wait(mutex_tcp_a); */
  /*     if (retour != 0) */
  /*     { */
  /*       perror(strerror(errno)); */
  /*       exit(1); */
  /*     } */
  /*   } */
  /*   printf("HERE WAIT END: %s\n", coup_adv); */
  /*   fflush(stdout); */
  /*   #<{(| A CHANFER LE -1 |)}># */
  /*   write(desc_pipe_write_a, coup_adv, strlen(coup_adv)-1); */
  /*   coup_adv[0] = 0; */
  /* } */
  /* buffer[read(desc_pipe_read_a, buffer, 12)] = 0; //SYNCHRO */
  /* buffer[read(desc_pipe_read_a, buffer, 12)] = 0; */
  /* if (strncmp(buffer, "ready", 5) == 0) */
  /*   send_coup_a(); */
  /* buffer[read(desc_pipe_read_a, buffer, 12)] = 0; //SYNCHRO */
  /* #<{(| printf("RETOUR%s\n",buffer); |)}># */

  return string_plat();


}

/* void receive_vote(char * coup) */
/* { */
/*   Vote * tmp = vote_debut; */
/*   while(tmp != NULL && tmp != 0){ */
/*     if(strcmp(tmp->coup,coup) ==0){ */
/*       tmp->nb_vote ++; */
/*       return; */
/*     } */
/*   } */
/*   Vote *v = malloc(sizeof(Vote)); */
/*   createElementLcV(coup,v); */
/*   addLcV(v); */
/* } */
/*  */
/* char * get_best(){ */
/*   Vote * tmp = vote_debut; */
/*   int max = 0; */
/*   char * coup = ""; */
/*    while(tmp != NULL && tmp != 0){ */
/*     if(max < tmp->nb_vote){ */
/*       coup = tmp->coup; */
/*       max = tmp->nb_vote; */
/*     } */
/*     tmp= tmp->next; */
/*   } */
/*   return coup; */
/* } */

void send_coup_a()
{
  char * coup = get_best();
  int i = 0;
  for (i = 0; i < strlen(coup); i++)
    coup_jouer[i] = coup[i];
  coup_jouer[i] = 0;
  sem_post(mutex_play_a);
  /* printf("COUP === %s\n", coup); */
  /* fflush(stdout); */
  //write(desc_pipe_write_a, coup, strlen(coup));
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
    sem_post(mutex_tcp_a);
    return 1;
  }
  return 0;
}

