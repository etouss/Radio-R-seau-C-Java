#include "com_diff.h"
extern int verbose;
void format_adress(char * new_adress, const char * old_adress){
  int i;
  int j = 1;
  int missing = 0;
  char tmp[TAILLE_ADDRESS + 2];
  sprintf(tmp, "%s", old_adress);
  for (i = strlen(tmp) - 1; i >= 0; i--){ 
    if (tmp[i] == '.' && (j + missing) % 4 != 0){
      for (; ((j + missing) % 4) != 0; missing++){
        new_adress[TAILLE_ADDRESS - j - missing] = '0';
      }
    }
    new_adress[TAILLE_ADDRESS - j - missing] = tmp[i];
    j++;
  } 
  new_adress[TAILLE_ADDRESS] = 0;
}
void format_id(char * new_id, const char * old_id){
  memset(new_id, '#', TAILLE_ID);
  new_id[TAILLE_ID] = 0;
  int i = 0;
  for (i = 0; i < strlen(old_id); i++)
    new_id[i] = old_id[i];
}
void format_port(char * new_port, const char * old_port){
  memset(new_port, '0', TAILLE_PORT);
  new_port[TAILLE_PORT] = 0;
  int i = 0;
  for (i = 0; i < strlen(old_port); i++)
    new_port[(4 - strlen(old_port)) + i] = old_port[i];
}
void format_regi(char * regi_message, char * reponse){
  char * pch;
  char new_id[TAILLE_ID+1];
  char new_adress_diff[TAILLE_ADDRESS+1];
  char new_adress_multi[TAILLE_ADDRESS+1];
  char new_port_multi[TAILLE_PORT+1];
  char new_port_diff[TAILLE_PORT+1];
  char old_id[TAILLE_ID+1];
  char old_adress_diff[TAILLE_ADDRESS+1];
  char old_adress_multi[TAILLE_ADDRESS+1];
  char old_port_multi[TAILLE_PORT+1];
  char old_port_diff[TAILLE_PORT+1];
  int i =0;
  pch = strtok (reponse," \r");
  while (i <= 6){
    i++;
    switch(i){
      case 1  :
        strcat(regi_message, pch);
        strcat(regi_message, " ");
        break;
      case 2  :
        strcpy(old_id, pch);
        format_id(new_id, old_id);
        strcat(regi_message, new_id);
        strcat(regi_message, " ");
        break; /* optional */
      case 3  :
        strcpy(old_adress_multi, pch);
        format_adress(new_adress_multi, old_adress_multi);
        strcat(regi_message, new_adress_multi);
        strcat(regi_message, " ");
        break; /* optional */
      case 4  :
        strcpy(old_port_multi, pch);
        format_port(new_port_multi , old_port_multi );
        strcat(regi_message, new_port_multi);
        strcat(regi_message, " ");
        break;
      case 5  :
        strcpy(old_adress_diff, pch);
        format_adress(new_adress_diff , old_adress_diff);
        strcat(regi_message, new_adress_diff);
        strcat(regi_message, " ");
        break;
      case 6  :
        strcpy(old_port_diff, pch);
        format_port(new_port_diff , old_port_diff);
        strcat(regi_message, new_port_diff);
        strcat(regi_message, " ");
        break;
      }
    pch = strtok (NULL, " \r");

  }
}
void ask_RUOK(Dlist* p_list, int descSock, struct node* p_node){
  int recu, envoie;
  char reponse[6];
  fcntl( descSock, F_SETFL, O_NONBLOCK);
  struct pollfd p[1];
  p[0].fd = descSock;
  p[0].events = POLLIN;
  while (42){
    sleep(15);
    envoie = send(descSock, "RUOK\r\n", strlen("RUOK") + 2 * sizeof(char), 0);
    printf("RUOK %s \n", p_node->id);
    if (envoie == -1){
      perror (strerror (errno));
      exit (1);
    }
    int ret = poll(p, 1, 2000);
    if (ret > 0){
      if (p[0].revents == POLLIN){
        recu = recv(descSock, reponse, 6 * sizeof(char), 0);
        if (recu >= 0){
          reponse[recu] = '\0';
          printf("Reçu de %s: %s", p_node->id, reponse);
        }
        else{
          perror(strerror (errno));
          exit(1);
        }
        if (strncmp(reponse, "IMOK", 4) != 0){
          printf("Diffuseur %s nonok: déconnecté\n", p_node->id);
          dlist_remove(p_list, p_node);
          return;
        }
      }
      else{
       	printf("Diffuseur revents != POLLIN: %s déconnecté\n", p_node->id);
        dlist_remove(p_list, p_node);
        return;
      }
    }
    else if(ret == 0){
    	printf("Diffuseur %s deconnecté: TIMEOUT\n", p_node->id);
      dlist_remove(p_list, p_node);
      return;
    }
    else{
      perror(strerror (errno));
      exit(1);
    }
  }
}
void register_diff(Dlist* p_list, int descSock , char* reponse){
  printf("Nouvelle connexion...\n");
  int retour;
  char id [TAILLE_ID+1];
  char ip1 [TAILLE_ADDRESS+1];
  char port1 [TAILLE_PORT+1];
  char ip2 [TAILLE_ADDRESS+1];
  char port2 [TAILLE_PORT+1];

  if (strlen(reponse) != 57){
    printf("WARNING: Mauvais format REGI\n");
    char regi_message[57];
    format_regi(regi_message, reponse);
    strcpy(reponse, regi_message);
  }
  else if(dlist_length(p_list)>= NBR_DIFF){
    retour = send(descSock, "RENO\r\n", strlen("RENO") + 2 * sizeof(char), 0);
    printf("Mauvais format de message reçu, reçu: %s\n", reponse);
    if (retour == -1){
      perror (strerror (errno));
      exit (1);
    }
  }
  snprintf(id,    TAILLE_ID+1,      "%s", reponse + REGI+1);
  if(verbose)
  printf("id :  %s\n", id);
  snprintf(ip1,   TAILLE_ADDRESS+1, "%s", reponse + REGI+1 + TAILLE_ID+1);
  if(verbose)
  printf("ip1 : %s\n", ip1);
  snprintf(port1, TAILLE_PORT+1,    "%s", reponse + REGI+1 + TAILLE_ID+1 + TAILLE_ADDRESS+1);
  if(verbose)
  printf("port1 : %s\n", port1);
  snprintf(ip2,   TAILLE_ADDRESS+1, "%s", reponse + REGI+1 + TAILLE_ID+1 + TAILLE_ADDRESS+1 + TAILLE_PORT+1);
  if(verbose)
  printf("ip2 : %s\n", ip2);
  snprintf(port2, TAILLE_PORT+1,    "%s", reponse + REGI+1 + TAILLE_ID+1 + TAILLE_ADDRESS+1 + TAILLE_PORT+1 + TAILLE_ADDRESS+1);
  if(verbose)
  printf("port2 : %s\n", port2);
  struct node* p_new;
  if ((p_new = dlist_append(p_list, id, ip1, port1, ip2, port2)) != NULL){
    retour = send(descSock, "REOK\r\n", strlen("REOK") + 2 * sizeof(char), 0);
    printf("REOK\n");
    if (retour == -1){
      perror (strerror (errno));
      exit (1);
    }
    ask_RUOK(p_list, descSock, p_new);
  }
  else{
      retour = send(descSock, "RENO\r\n", strlen("RENO") + 2 * sizeof(char), 0);
      printf("RENO\n");
  }
}
