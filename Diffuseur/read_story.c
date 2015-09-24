#include "read_story.h"

char * read_story()
{
  static int file = 0;
  char * tampon = malloc(sizeof(char) * TAILLE_MESSAGE + 1);
  int rc = 0;
  if (!file)
  {
    char * nom_file = "Story.txt";
    if ((file = open(nom_file, O_RDONLY)) == -1) errx(1, "impossible d ouvrir l archive %s \n", nom_file);
  }

  rc = read(file, tampon, TAILLE_MESSAGE);
  if(rc < TAILLE_MESSAGE)
    lseek(file,0,SEEK_SET);
  tampon[rc] = 0;
  return tampon;
};

int process_input_client_read_story(char * buff, int descSock, char * address_caller)
{
  int retour = 0;
  char  mess[TAILLE_MAX_TCP];
  if (strncmp(buff, "INFO", 4) == 0)
  {
    format_mess_desc(mess, "Une joli Histoire raconté n'hesiter pas a la commenter !!!");
    retour = send(descSock, mess, strlen(mess), 0);
    if (retour == -1)
    {
      perror(strerror(errno));
      /* exit(1); */
    }

    return 1;
  }
  return 0;
}
