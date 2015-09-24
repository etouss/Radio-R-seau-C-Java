#include "write_story.h"

typedef struct story
{
  char story[TAILLE_MESSAGE + 1];
  char ip[TAILLE_ADDRESS + 1];
  struct story * next;
} Story;

Story* story_debut;
Story* story_fin;
char last_ip[TAILLE_ADDRESS + 1];
int nb_sub = 0;

void createElementLcS(char * story, char * ip , Story * s)
{
  int i = 0;
  for (i = 0; i < strlen(story) && i < TAILLE_MESSAGE; i++)
    s->story[i] = story[i];
  for (i = 0; i < strlen(ip) && i < TAILLE_ADDRESS; i++)
    s->ip[i] = ip[i];
  s->story[i] = 0;
  s->next = NULL;
}

void addLcS(Story * s)
{
  nb_sub ++;
  if (story_debut == 0)
  {
    story_debut = s;
    story_fin = s;
    return;
  }
  story_fin->next = s;
  story_fin = s;
}

void raz_story()
{
  Story * tmp = story_debut;
  while (tmp != NULL && tmp != 0)
  {
    Story *next = tmp->next;
    free(tmp);
    tmp = next;
  }
  story_fin = 0;
  story_debut = 0;
  nb_sub = 0;
}

void receive_story(char * story, char * address_caller)
{
  if (strncmp(address_caller, last_ip, strlen(address_caller)) == 0)return;
  Story *s = malloc(sizeof(Story));
  createElementLcS(story, address_caller, s);
  addLcS(s);
}

void get_story(char * tampon)
{
  int nombre_aleatoire = rand() % nb_sub;
  int i = 0;
  Story * tmp = story_debut;
  for (i = 0; i < nombre_aleatoire; i++)
    tmp = tmp->next;
  for (i = 0; i < strlen(tmp->story); i++)
    tampon[i] = tmp->story[i];
  tampon[i] = 0;
  for (i = 0; i < strlen(tmp->ip); i++)
    last_ip[i] = tmp->ip[i];
  last_ip[i] = 0;
  raz_story();
}


char * write_story()
{
  static int file = 0;
  char * tampon = malloc(sizeof(char) * 141);
  if (!file)
  {
    char * nom_file = "story_write.txt";
    if ((file = open(nom_file, O_WRONLY | O_CREAT | O_TRUNC, 0600)) == -1) errx(1, "impossible d ouvrir le fichier  %s \n", nom_file);
  }
  if (nb_sub == 0)
  {
    last_ip[0] = 0;
    sprintf(tampon, "Participer SVP!");
    /* printf("%s\n",tampon); */
    /* fflush(stdout); */
    return tampon;
  }
  get_story(tampon);
  /* printf("%s\n",tampon); */
  /* fflush(stdout); */
  write(file, tampon, strlen(tampon));
  write(file, "\n", strlen("\n"));
  return tampon;
};

int process_input_client_write_story(char * buff, int descSock, char * address_caller)
{
  int retour = 0;
  int set = 1;
  setsockopt(descSock, SOL_SOCKET, 13, (void *)&set, sizeof(int));
  char  mess[TAILLE_MAX_TCP];
  if (strncmp(buff, "INFO", 4) == 0)
  {
    format_mess_desc(mess, "Une radio d'écriture d'histoire communautaire, vous ne pouvez pas écrire deux fois de suite !");
    send(descSock, mess, strlen(mess), 0);
    send(descSock, "VOTE\r\n", strlen("VOTE\r\n"), 0);
    send(descSock, "DOWN\r\n", strlen("DOWN\r\n"), 0);
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
    if (strncmp(buff + 5, "VOTE", 4) == 0)
    {
      format_mess_help(mess, "Commande pour ecrire : [VOTE MESSAGEAECRIRE] ou MESSAGE A ECRIRE FAIT AU  plus 140 CARACTERE", "VOTE");
      retour = send(descSock, mess, strlen(mess), 0);
      if (retour == -1)
      {
        perror(strerror(errno));
        /* exit(1); */
      }

      return 1;
    }
    else if (strncmp(buff + 5, "DOWN", 4) == 0)
    {
      format_mess_help(mess, "Commande pour télécharger l'histoire que vous avez creer !", "DOWN");
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
    char * end = strrchr(buff, '\r');
    if(end != NULL)
      end[0] = 0;
    receive_story(buff + 5, address_caller);
    return 1;

  }

  else if (strncmp(buff, "DOWN", 4) == 0)
  {
      format_mess_help(mess, story_debut->story, "HIST");
      send(descSock, mess, strlen(mess), 0);
      retour = send(descSock, "ENDM\r\n", strlen("ENDM\r\n"), 0);
      if (retour == -1)
        perror(strerror(errno));
      return 1;
  }
  return 0;
}
