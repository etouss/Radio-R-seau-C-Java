
#include "vote_lib.h"


Vote* vote_debut;
Vote* vote_fin;

void createElementLcV(char * coup, Vote * v)
{
    int i = 0;
    for(i=0; i<strlen(coup)&&i<12; i++)
        {
            v->coup[i] = coup[i];
            v->nb_vote =1;
        }
    v->coup[12]=0;
    v->next = NULL;
}

void addLcV(Vote * v)
{
    if(vote_debut == 0)
        {
            vote_debut = v;
            vote_fin = v;
            return;
        }
    vote_fin->next = v;
    vote_fin = v;
}

void raz_vote()
{
  Vote * tmp = vote_debut;
  while(tmp != NULL && tmp != 0){
    Vote *next = tmp->next;
    free(tmp);
    tmp = next;
  }
  vote_fin = 0;
  vote_debut = 0;
}

void receive_vote(char * coup)
{
  Vote * tmp = vote_debut;
  while(tmp != NULL && tmp != 0){
    if(strcmp(tmp->coup,coup) ==0){
      tmp->nb_vote ++;
      return;
    }
  }
  Vote *v = malloc(sizeof(Vote));
  createElementLcV(coup,v);
  addLcV(v);
}

char * get_best(){
  Vote * tmp = vote_debut;
  int max = 0;
  char * coup = "vide\r\n";
   while(tmp != NULL && tmp != 0){
    if(max < tmp->nb_vote){
      coup = tmp->coup;
      max = tmp->nb_vote;
    }
    tmp= tmp->next;
  }
  return coup;
}

