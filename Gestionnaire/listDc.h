#ifndef __listDc__
#define __listDc__

#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <errno.h>
#include <unistd.h>
#include <string.h>
#include <sys/socket.h>
#include <netdb.h>
#include <pthread.h>
struct node{
	char id[9];
	char ip1[16];
	char port1[5];
	char ip2[16];
	char port2[5];
  struct node *p_next;
  struct node *p_prev;
};

typedef struct dlist{
  size_t length;
  struct node *p_tail;
  struct node *p_head;
} Dlist;
#define NBR_DIFF 5
#endif

Dlist *dlist_new(void);
struct node* dlist_append(Dlist*, char*, char*, char*, char*, char*);
void dlist_delete(Dlist **);
void dlist_remove(Dlist *, struct node*);
int dlist_find(Dlist *, char*);
size_t dlist_length(Dlist *);
void parcours_liste(Dlist *);
