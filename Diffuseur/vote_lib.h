

#ifndef __vote_lib__
#define __vote_lib__

#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <errno.h>
#include <unistd.h>
#include <err.h>
#include <string.h>
#include <fcntl.h>
#include <string.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netdb.h>

#include <pthread.h>
#include <errno.h>
#include "debug.h"


typedef struct vote
{
    char coup[13];
    int nb_vote;
    struct vote * next;
} Vote;


#endif

void createElementLcV(char * coup, Vote * v);
void addLcV(Vote * v);
void raz_vote();
void receive_vote(char * coup);
char * get_best();
