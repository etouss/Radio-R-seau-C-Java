
#include "guess_number.h"

pthread_mutex_t alarm_lock= PTHREAD_MUTEX_INITIALIZER;
extern const char * id;

int win = 0;
int nombre_aleatoire = 0;

typedef struct Fifo
{
  char addr [TAILLE_ADDRESS + 1];
  struct Fifo * next;
} Fifo;

Fifo* fifoDebut;
Fifo* fifoFin;

void createElementFifo(char *addr, Fifo * f)
{
  strcpy(f->addr, addr);
  f->next = NULL;
}

void addFifo(Fifo * f)
{
  if (fifoDebut == NULL || fifoDebut == 0)
  {
    fifoDebut = f;
    fifoFin = f;
  }
  else
  {
    fifoFin->next = f;
    fifoFin = f;
  }
}

void popFifo()
{
  Fifo * tmpFifo = fifoDebut->next;
  free(fifoDebut);
  fifoDebut = tmpFifo;
}

void alarm_handler (int signum)
{
  pthread_mutex_lock(&alarm_lock);
  popFifo();
  pthread_mutex_unlock(&alarm_lock);
}

int check_caller(char * caller)
{
  Fifo * tmpFifo = fifoDebut;
  while (tmpFifo != NULL && tmpFifo != 0)
  {
    if (strcmp(caller, tmpFifo->addr) == 0)return 0;
    tmpFifo = tmpFifo->next;
  }
  return 1;
}

void receive_play(char * nbre, char * caller)
{
  char *ptr;
  long ret;
  ret = strtol(nbre, &ptr, 10);
  if (ptr != nbre + strlen(nbre) || ret > 9999)
    return ;
  int nb = atoi(nbre);
  if (!check_caller(caller))
    return;
  if (nb == nombre_aleatoire)
  {
    char mess[TAILLE_MESSAGE + 1];
    sprintf(mess, "%s a gagné : le nombre était %d", caller, nombre_aleatoire);
    send_message(mess,id);
    win = 1;
    return;
  }
  else if (nb > nombre_aleatoire)
  {
    char mess[TAILLE_MESSAGE + 1];
    sprintf(mess, "le nombre est plus petit que %d", nb);
    send_message(mess,id);
  }
  else if (nb < nombre_aleatoire)
  {
    char mess[TAILLE_MESSAGE + 1];
    sprintf(mess, "le nombre est plus grand que %d", nb);
    send_message(mess,id);
  }
  Fifo *f = malloc(sizeof(Fifo));
  createElementFifo(caller, f);
  addFifo(f);
  alarm(10);
}


char * send_recap_guess()
{
  static int first = 1;
  if (first)
  {
    struct sigaction new_action;
    new_action.sa_handler = alarm_handler;
    sigemptyset (&new_action.sa_mask);
    new_action.sa_flags = 0;
    sigaction (SIGALRM, &new_action, NULL);
    srand(time(NULL));
    nombre_aleatoire = rand() % 1000;
    printf("%d\n", nombre_aleatoire);
    first = !first;
  }
  if (win)
  {
    /* A verifier si ca marche vis a vis du alarm */
    sleep(10);
    nombre_aleatoire = rand() % 1000;
    printf("%d\n", nombre_aleatoire);
    win = 0;
  }
  char * tamp = malloc(sizeof(char) * 20);
  sprintf(tamp, "Guess number !!");
  return tamp;
}

int process_input_client_guess_number(char * buff, int descSock, char * address_caller)
{
  int set = 1;
  setsockopt(descSock, SOL_SOCKET, 13, (void *)&set, sizeof(int));
  int retour = 0;

  char  mess[TAILLE_MAX_TCP];
  if (strncmp(buff, "INFO", 4) == 0)
  {
    format_mess_desc(mess, "Un simple jeu ou vous devez deviner le nombre caché");
    send(descSock, mess, strlen(mess), 0);
    send(descSock, "PLAY\r\n", strlen("PLAY\r\n"), 0);
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
    if (strncmp(buff + 5, "PLAY", 4) == 0)
    {
      format_mess_help(mess, "Commande pour jouer : [PLAY nombre<9999] ou nombre est écrit sur 4 octect", "PLAY");
      retour = send(descSock, mess, strlen(mess), 0);
      if (retour == -1)
      {
        perror(strerror(errno));
        /* exit(1); */
      }

      return 1;
    }
  }
  else if (strncmp(buff, "PLAY", 4) == 0)
  {
    char * end = strrchr(buff, '\r');
    if(end != NULL)
      end[0] = 0;
    receive_play(buff + 5, address_caller);
    return 1;

  }
  return 0;

}
