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
extern char rdy ;
extern char coup_part[13];
extern pthread_mutex_t synchro_mutex;
extern pthread_cond_t synchro_cv;

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

void interaction(char * coup , pion ** grille , char* joueur , int mode, struct coup * pere)
{
  pthread_mutex_lock(&synchro_mutex); 
  while (coup_part[0] == 0){
    #ifdef DEBUG
    printf("Gounki is waiting for coup\n");
    #endif
    pthread_cond_wait(&synchro_cv, &synchro_mutex);
  }
  strcpy(coup, coup_part);
  coup[strlen(coup)-2]=0;
  coup = correrction(grille, coup, pere);
}

void * jouer_radio_IA(void * rdy_p)
{
  char *coup; 
  struct coup * pere;
  char joueur;
  int i;
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
  pthread_mutex_lock(&synchro_mutex);
  rdy = 1;
  pthread_cond_signal(&synchro_cv);
  #ifdef DEBUG
  printf("Gounki is ready\n");
  #endif

  pthread_mutex_unlock(&synchro_mutex);


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
        interaction(coup, grille, &joueur, mode, pere);
        strcpy(coup_part, coup);
        if (action(grille, coup, pere, 1) != -1)break;
        pthread_cond_signal(&synchro_cv);
        rdy = 1;
        #ifdef DEBUG
        printf("Gounki is ready\n");
        #endif
        pthread_mutex_unlock(&synchro_mutex);
        coup_part[0]=0;
        /* updateMap(grille); */
      }
      while (42);
      freeCoup(pere, 1);
      joueur = 'B';
    }
    if (joueur == 'B')
    {
    if (!victoire)
      {
        jouerCoupIA(grille, 'B', difficulter, 0, 0, type, coup);
        joueur = 'A';
        updateMap(grille);
        rdy = 1;
        pthread_cond_signal(&synchro_cv);
        #ifdef DEBUG
        printf("Gounki is ready\n");
        #endif
        pthread_mutex_unlock(&synchro_mutex);      }
    }
  }
  freeMap();
  free(grille);
  free(coup);
  return NULL;
}


void * jouer_radio_VS(void * rdy_p)
{
  char *coup;
  /* void ** rdy = (void **)rdy_p; */
  /* int * desc_pipe_write = (int *)rdy[0]; */
  /* int * desc_pipe_read = (int *)rdy[1]; */
  struct coup * pere;
  char joueur;
  int i;
  /* int pipeDes_write[2]; */
  /* pipe(pipeDes_write); */
  /* int pipeDes_read[2]; */
  /* pipe(pipeDes_read); */
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
  /* * desc_pipe_write = pipeDes_write[1]; */
  /* * desc_pipe_read = pipeDes_read[0]; */
  /* free(rdy_p); */
  updateMap(grille);
  pthread_mutex_lock(&synchro_mutex);
  rdy = 1;
  pthread_cond_signal(&synchro_cv);
  #ifdef DEBUG
  printf("Gounki is ready\n");
  #endif

  pthread_mutex_unlock(&synchro_mutex);
 
  //GERER LE PREMIER SYNCHRO

  while (!victoire)
  {
    ready_to_play = 0;
    if (joueur == 'A')
    {
      /* updateMap(grille); */
      //affiche()
      sleep(1);
      pere = malloc(sizeof(struct coup));
      pere->proto = NULL;
      pere->coupSuivant = NULL;
      pere = coupsPossibles(grille, 'A', pere);
      /* write(pipeDes_read[1],"ready",strlen("ready")); */
      interaction(coup, grille, &joueur, mode, pere);
      if (action(grille, coup, pere, 1) == -1)
        jouerCoupIA(grille, 'A', difficulter, 0, 0, type, coup);
      strcpy(coup_part, coup);
      updateMap(grille);
      rdy = 1;
      pthread_cond_signal(&synchro_cv);
      #ifdef DEBUG
      printf("Gounki is ready\n");
      #endif
      pthread_mutex_unlock(&synchro_mutex);
      /* write(pipeDes_read[1],"bad_coup_in",strlen("bad_coup_in_")); */
      freeCoup(pere, 1);
      joueur = 'B';
    }
    if (joueur == 'B')
    {
      //affiche();

      if (!victoire)
      {
      sleep(1);//affiche()
      pere = malloc(sizeof(struct coup));
      pere->proto = NULL;
      pere->coupSuivant = NULL;
      pere = coupsPossibles(grille, 'B', pere);
      /* write(pipeDes_read[1],"ready",strlen("ready")); */
      interaction(coup, grille, &joueur, mode, pere);
      if (action(grille, coup, pere, 1) == -1)
        jouerCoupIA(grille, 'B', difficulter, 0, 0, type, coup);
      strcpy(coup_part, coup);
      updateMap(grille);
      rdy = 1;
      pthread_cond_signal(&synchro_cv);
      #ifdef DEBUG
      printf("Gounki is ready\n");
      #endif
      pthread_mutex_unlock(&synchro_mutex);
      /* write(pipeDes_read[1],"bad_coup_in",strlen("bad_coup_in_")); */
      freeCoup(pere, 1);
      joueur = 'A';

      }
    }
  }
  freeMap();
  free(grille);
  free(coup);
  /* close(pipeDes_write[1]); */
  /* close(pipeDes_write[0]); */
  /* close(pipeDes_read[1]); */
  /* close(pipeDes_read[0]); */
  return NULL;
}

