/*
//  Jouer.c
//  Gounki
//
//  Created by andres quiroz on 30/04/2014.
//  Copyright (c) 2014 andres quiroz. All rights reserved.
*/

#include <stdio.h>
#include <stdlib.h>
#include "Jouer.h"
/*1=48,8=56*/
/*a=97,h=104*/

int victoire = 0;
int ready_to_play = 0;
pion ** grilleCancel;
pion ** grilleReDo;

char * correrction(pion ** grille, char * proto, coup* pere)
{
  if (proto[2] == '*' || proto[2] == '+')
  {
    pion ** grilleCpy = copieGrille(grille);
    char * protoA = calloc(5, sizeof(char));
    char * protoB = calloc(8, sizeof(char));
    int i = 0;
    switch (strlen(proto))
    {
    case 8:
      if (action(grilleCpy, proto, pere, 0) != -1)
      {
        break;
      }
      for (i = 0; i < 5; i++)
      {
        protoA[i] = proto[i];
      }

      if (action(grilleCpy, protoA, pere, 0) != -1)
      {
        proto[5] = 0;
        break;
      }

      for (i = 3; i < 5; i++)
      {
        protoA[i] = proto[i + 3];
      }

      if (action(grilleCpy, protoA, pere, 0) != -1)
      {
        for (i = 0; i < 5; i++)
        {
          proto[i] = protoA[i];
        }
        proto[5] = 0;
        break;
      }
      break;
    case 11:
      for (i = 0; i < 5; i++)
      {
        protoA[i] = proto[i];
      }

      if (action(grilleCpy, protoA, pere, 0) != -1)
      {
        proto[5] = 0;
        break;
      }

      for (i = 3; i < 5; i++)
      {
        protoA[i] = proto[i + 3];
      }

      if (action(grilleCpy, protoA, pere, 0) != -1)
      {
        for (i = 0; i < 5; i++)
        {
          proto[i] = protoA[i];
        }
        proto[5] = 0;
        break;
      }

      for (i = 3; i < 5; i++)
      {
        protoA[i] = proto[i + 6];
      }

      if (action(grilleCpy, protoA, pere, 0) != -1)
      {
        for (i = 0; i < 5; i++)
        {
          proto[i] = protoA[i];
        }
        proto[5] = 0;
        break;
      }

      for (i = 0; i < 8; i++)
      {
        protoB[i] = proto[i];
      }
      if (action(grilleCpy, protoB, pere, 0) != -1)
      {
        proto[8] = 0;
        break;
      }

      for (i = 5; i < 8; i++)
      {
        protoB[i] = proto[i + 3];
      }
      if (action(grilleCpy, protoB, pere, 0) != -1)
      {
        for (i = 0; i < 8; i++)
        {
          proto[i] = protoB[i];
        }
        proto[8] = 0;
        break;
      }


      for (i = 3; i < 8; i++)
      {
        protoB[i] = proto[i + 3];
      }

      if (action(grilleCpy, protoB, pere, 0) != -1)
      {
        for (i = 0; i < 8; i++)
        {
          proto[i] = protoB[i];
        }
        proto[8] = 0;
        break;
      }
      break;
    }
    free(protoA);
    protoA = NULL;
    free(protoB);
    protoB = NULL;
    freeGrille(grilleCpy);
  }
  return proto;
}

void setVictoire(int v)
{
  victoire = v;
}


void viderBuffer()
{
  int c = 0;
  while (c != '\n' && c != EOF)
  {
    c = getchar();
  }
}

void copy_str(char * dst, char * src)
{
  int i = 0;
  for (i = 0; i < strlen(src); i++)
    dst[i] = src[i];
  dst[i] = 0;
}


void interaction(char * coup , pion ** grille , char* joueur , int mode, struct coup * pere)
{
  extern sem_t * mutex_play_a;
  extern char coup_jouer[13];
  printf("LOCKR\n");
  fflush(stdout);
  sem_wait(mutex_play_a);
  copy_str(coup_jouer, coup);

  /* int len; */
  /* len = read(pipe, coup, 13); */
  /* //fgets(coup, 13, stdin); */
  /* coup[len] = 0; */
  coup = correrction(grille, coup, pere);
  /* printf("COUP ===> %s|||",coup); */

  /* int len; */
  /* read(0,coup,13) */
  /* fgets(coup, 13, stdin); */
  /* len = (int)strlen(coup); */
  /* coup[len-1]=0; */
  /* coup = correrction(grille, coup, pere); */
}


void * jouer_radio_IA(void * rdy_p)
{
  char *coup;
  void ** rdy = (void **)rdy_p;
  int * desc_pipe_write = (int *)rdy[0];
  int * desc_pipe_read = (int *)rdy[1];
  struct coup * pere;
  char joueur;
  int i;
  int pipeDes_write[2];
  pipe(pipeDes_write);
  int pipeDes_read[2];
  pipe(pipeDes_read);  /* close(pipeDes[0]); */
  pion ** grille;
  initMap();
  joueur = 'A';
  grille = initGrille();
  coup = malloc(sizeof(char) * 13);
  for (i = 0; i < 13; i++)
  {
    coup[i] = 0;
  }

  int mode = 2;
  int difficulter = 1;
  int type = 0;
  * desc_pipe_write = pipeDes_write[1];
  * desc_pipe_read = pipeDes_read[0];
  free(rdy_p);
  updateMap(grille);


  while (!victoire)
  {
    ready_to_play = 0;
    if (joueur == 'A')
    {
      /* updateMap(grille); */
      //affiche();
      pere = malloc(sizeof(struct coup));
      pere->proto = NULL;
      pere->coupSuivant = NULL;
      pere = coupsPossibles(grille, 'A', pere);
      /* updateMap(grille); */
      do
      {
        write(pipeDes_read[1], "ready", strlen("ready"));
        interaction(coup, grille, &joueur, mode, pere);
        if (action(grille, coup, pere, 1) != -1)break;
        /* updateMap(grille); */
        write(pipeDes_read[1], "bad_coup_in", strlen("bad_coup_in_"));
      }
      while (42);
      freeCoup(pere, 1);
      joueur = 'B';
    }
    if (joueur == 'B')
    {
      /* updateMap(grille); */
      //affiche();

      if (!victoire)
      {
        jouerCoupIA(grille, 'B', difficulter, 0, 0, type, coup);
        joueur = 'A';
        updateMap(grille);
        write(pipeDes_read[1], coup, strlen(coup));
      }
    }
  }
  freeMap();
  free(grille);
  free(coup);
  close(pipeDes_write[1]);
  close(pipeDes_write[0]);
  close(pipeDes_read[1]);
  close(pipeDes_read[0]);
  return NULL;
}


void * jouer_radio_VS(void * rdy_p)
{
  extern sem_t * mutex_play_a;
  extern char coup_jouer[13];
  char *coup;
  struct coup * pere;
  char joueur;
  int i;
  /* close(pipeDes[0]); */
  pion ** grille;
  initMap();
  joueur = 'A';
  grille = initGrille();
  coup = malloc(sizeof(char) * 13);
  for (i = 0; i < 13; i++)
  {
    coup[i] = 0;
  }

  int mode = 2;
  int difficulter = 1;
  int type = 0;
  updateMap(grille);
  sem_post(mutex_play_a);

  while (!victoire)
  {
    ready_to_play = 0;
    if (joueur == 'A')
    {
      /* updateMap(grille); */
      //affiche()
      pere = malloc(sizeof(struct coup));
      pere->proto = NULL;
      pere->coupSuivant = NULL;
      pere = coupsPossibles(grille, 'A', pere);
      /* write(pipeDes_read[1],"ready",strlen("ready")); */
      printf("UNLOCK1\n");
      fflush(stdout);
      sem_post(mutex_play_a);
      interaction(coup, grille, &joueur, mode, pere);
      if (action(grille, coup, pere, 1) == -1)
        jouerCoupIA(grille, 'A', difficulter, 0, 0, type, coup);
      copy_str(coup_jouer, coup);
      printf("UNLOCK2\n");
      fflush(stdout);
      sem_post(mutex_play_a);
      freeCoup(pere, 1);
      joueur = 'B';
    }
    if (joueur == 'B')
    {
      updateMap(grille);
      //affiche();

      if (!victoire)
      {
        //affiche()
        pere = malloc(sizeof(struct coup));
        pere->proto = NULL;
        pere->coupSuivant = NULL;
        pere = coupsPossibles(grille, 'B', pere);
        /* write(pipeDes_read[1],"ready",strlen("ready")); */
        printf("UNLOCK3\n");
        fflush(stdout);

        sem_post(mutex_play_a);
        interaction(coup, grille, &joueur, mode, pere);
        if (action(grille, coup, pere, 1) == -1)
          jouerCoupIA(grille, 'B', difficulter, 0, 0, type, coup);
        copy_str(coup_jouer, coup);
        printf("UNLOCK4\n");
        fflush(stdout);

        sem_post(mutex_play_a);
        freeCoup(pere, 1);
        joueur = 'A';

      }
    }
  }
  freeMap();
  free(grille);
  free(coup);
  return NULL;
}

