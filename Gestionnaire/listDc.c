#include "listDc.h"

pthread_mutex_t verrou = PTHREAD_MUTEX_INITIALIZER;

Dlist *dlist_new(void){
  Dlist *p_new = malloc(sizeof *p_new);
  if (p_new != NULL){
    p_new->length = 0;
    p_new->p_head = NULL;
    p_new->p_tail = NULL;
  }
  return p_new;
}

struct node * dlist_append(Dlist* p_list, char* id,char* ip1, char* port1, char* ip2, char* port2){
	pthread_mutex_lock(&verrou);
  printf("Insertion de l'élement %s dans la liste des diffuseurs...\n", id);
	if(!dlist_find(p_list, id) && (dlist_length(p_list)<NBR_DIFF)){	
	  if (p_list != NULL){
  	  struct node *p_new = malloc(sizeof *p_new);
    	if (p_new != NULL){
    		strcpy(p_new->id, 		id);
				strcpy(p_new->ip1, 		ip1);
				strcpy(p_new->port1, 	port1);
				strcpy(p_new->ip2, 		ip2);	
				strcpy(p_new->port2, 	port2);
				p_new->p_prev=				NULL;
      	p_new->p_next=		 		NULL;
      	if (p_list->p_tail == NULL){
	        p_new->p_prev = NULL;
  	      p_list->p_head = p_new;
    	    p_list->p_tail = p_new;
      	}
      	else{
	        p_list->p_tail->p_next = p_new;
 	  	    p_new->p_prev = p_list->p_tail;
  	      p_list->p_tail = p_new;
      	}
        p_list->length++;
    	}
    pthread_mutex_unlock(&verrou);
  	return p_new;
 	 	}	
	}
  else if(dlist_find(p_list,id)){
  	printf("Cet élément est deja dans la liste des diffuseurs: %s\n", id);
		pthread_mutex_unlock(&verrou);
  	return NULL;
  }
  else{
  	pthread_mutex_unlock(&verrou);
  	return NULL;
	}
  pthread_mutex_unlock(&verrou);
 	return NULL;
}

void dlist_delete(Dlist **p_list){
  if (*p_list != NULL){
    struct node *p_temp = (*p_list)->p_head;
    while (p_temp != NULL){
      struct node *p_del = p_temp;
      p_temp = p_temp->p_next;
      free(p_del);
    }
    free(*p_list), *p_list = NULL;
  }
}

void dlist_remove(Dlist *p_list, struct node * p_node){
	pthread_mutex_lock(&verrou);
	if (p_list != NULL){
		if (p_node->p_prev == NULL)
			p_list->p_head = p_node->p_next;
		else 
			p_node->p_prev->p_next = p_node->p_next;
		if (p_node->p_next == NULL)
			p_list->p_tail = p_node->p_prev;
		else
			p_node->p_next->p_prev = p_node->p_prev;
		p_list->length--;
		free(p_node);
	}
	pthread_mutex_unlock(&verrou);
}
size_t dlist_length(Dlist *p_list){
  size_t ret = 0;
  if (p_list != NULL){
    ret = p_list->length;
  }
  return ret;
}
int dlist_find(Dlist *p_list, char* id){
  int found = 0;
  if (p_list != NULL){
	  struct node *p_temp = p_list->p_head;
	  while (p_temp != NULL && !found){
      if (strncmp((p_temp->id), id, strlen(id))==0){
        found = 1;
      }
      else{
        p_temp = p_temp->p_next;
      }
    }
  }
  return found;
}
void parcours_liste(Dlist *p_list){
	struct node *p_temp = p_list->p_head;
	printf("Liste des radios disponibles: \n");
	while (p_temp != NULL){
		printf("%s\n",p_temp->id);
		p_temp= p_temp->p_next;
	}

}
