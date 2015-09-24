#include "com_client.h"
pthread_mutex_t verrouG = PTHREAD_MUTEX_INITIALIZER;
extern int verbose;
void format_num_diff (char *n_num, int num){
  char nb[3];
  sprintf (nb, "%d", num);
  memset (n_num, '0', 2);
  n_num[2] = 0;
  int i = 0;
  for (i = 0; i < strlen (nb); i++)
    n_num[(2 - strlen (nb)) + i] = nb[i];
}
void lister(Dlist *p_list, int descSock){
	char linb[1024];
  char num[3];
  int retour;
  format_num_diff (num, dlist_length(p_list));
  sprintf (linb, "LINB %s\r\n", num);
  retour = send (descSock, linb, strlen (linb) * sizeof (char), 0);
  if (verbose)
  printf("%s\n", linb);
  if (retour == -1){
  	perror (strerror (errno));
    exit (1);
  }
  pthread_mutex_lock (&verrouG);
  struct node* p_tmp = p_list->p_head;

  if (p_tmp == NULL){
  	printf("La liste des messages est vide\n");
  }
  while (!(p_tmp == NULL || p_tmp == 0)){
    char mess[1024];
    sprintf (mess, "ITEM %s %s %s %s %s\r\n", p_tmp->id, p_tmp->ip1, p_tmp->port1, p_tmp->ip2, p_tmp->port2);
    retour = send (descSock, mess, strlen (mess) * sizeof (char), 0);
    if(verbose)
    printf("%s\n", mess);
    if (retour == -1){
      perror (strerror (errno));
      exit (1);
    }
    p_tmp = p_tmp->p_next;
  }
  pthread_mutex_unlock (&verrouG);
}
