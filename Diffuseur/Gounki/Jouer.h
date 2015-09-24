/*
//  Jouer.h
//  ProjetRC
//
//  Created by Etienne Toussaint on 08/05/2014.
//  Copyright (c) 2014 Etienne Toussaint. All rights reserved.
*/

#ifndef ProjetRC_Jouer_h
#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <errno.h>
#include <unistd.h>
#include <err.h>
#include <string.h>
#include <fcntl.h>
#include "grille.h"
#include "test.h"
#include "string.h"
#include "partieHeuristique.h"
#include "persistance.h"
#include <semaphore.h>
#include "../gounki_a.h"
#include "../gounki_ia.h"
#include "../debug.h"
#define ProjetRC_Jouer_h
void setVictoire(int v);
void * jouer_radio_IA(void * rdy_p);
void * jouer_radio_VS(void * rdy_p);
void setAdvice(char * ad);
void setGrilleCancel(pion ** grille);
char * correrction(pion ** grille,char * proto,coup* pere);


#endif
