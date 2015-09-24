#include "format_mess.h"

int check_terminaison_mess(char * mess)
{
#ifdef NC
  if (mess[strlen(mess) - 1] == '\n' )return 1;
#endif
  if (mess[strlen(mess) - 1] != '\n'  || mess[strlen(mess) - 2] != '\r' )return 0;
  return 1;
}

void format_adress(char * new_adress, const char * old_adress)
{
  int i;
  int j = 1;
  int missing = 0;
  char tmp[TAILLE_ADDRESS + 2];
  sprintf(tmp, "%s", old_adress);
  for (i = strlen(tmp) - 1; i >= 0; i--)
  { 
    if (tmp[i] == '.' && (j + missing) % 4 != 0)
    {
      for (; ((j + missing) % 4) != 0; missing++)
      {
        new_adress[TAILLE_ADDRESS - j - missing] = '0';
      }
    }
    new_adress[TAILLE_ADDRESS - j - missing] = tmp[i];
    j++;
  } 
  new_adress[TAILLE_ADDRESS] = 0;
}

void format_id(char * new_id, const char * old_id)
{
  memset(new_id, '#', TAILLE_ID);
  new_id[TAILLE_ID] = 0;
  int i = 0;
  for (i = 0; i < strlen(old_id); i++)
    new_id[i] = old_id[i];
  /* printf("%s\n",new_id); */
}

void format_port(char * new_port, const char * old_port)
{
  memset(new_port, '0', TAILLE_PORT);
  new_port[TAILLE_PORT] = 0;
  int i = 0;
  for (i = 0; i < strlen(old_port); i++)
    new_port[(4 - strlen(old_port)) + i] = old_port[i];
}

void format_num(char * n_num, int num)
{
  char nb[5];
  sprintf(nb, "%d", num);
  memset(n_num, '0', TAILLE_NUM_MESS);
  n_num[TAILLE_NUM_MESS] = 0;
  int i = 0;
  for (i = 0; i < strlen(nb); i++)
    n_num[(4 - strlen(nb)) + i] = nb[i];

}

void format_mess(char * n_mess, char * mess)
{
  int retour = 0;
  memset(n_mess, '#', TAILLE_MESSAGE);
  n_mess[TAILLE_MESSAGE] = 0;
  retour = snprintf(n_mess, TAILLE_MESSAGE + 1, "%s", mess);
  /* printf("%d\n",retour); */
  if (retour < TAILLE_MESSAGE)n_mess[retour] = '#';
}

void format_regi(char * mess, const char * nom, const char * addresse_diffuseur_multi, const char * port_diffuseur_multi, char * adresse_diffuseur_tcp , const char *port_diffuseur_tcp)
{
  /* [REGI␣id␣ip1␣port1␣ip2␣port2] */
  char  n_address_diffuseur_tcp[TAILLE_ADDRESS + 1];
  char  n_address_diffuseur_multi[TAILLE_ADDRESS + 1];
  char  n_id[TAILLE_ID + 1];
  char  n_port_diffuseur_multi[TAILLE_PORT + 1];
  char  n_port_diffuseur_tcp[TAILLE_PORT + 1];
  format_adress(n_address_diffuseur_tcp, adresse_diffuseur_tcp);
  format_adress(n_address_diffuseur_multi, addresse_diffuseur_multi);
  format_id(n_id, nom);
  format_port(n_port_diffuseur_tcp, port_diffuseur_tcp);
  format_port(n_port_diffuseur_multi, port_diffuseur_multi);
  sprintf(mess, "REGI %s %s %s %s %s\r\n", n_id, n_address_diffuseur_multi, n_port_diffuseur_multi, n_address_diffuseur_tcp, n_port_diffuseur_tcp);
}

void format_mess_udp(char * paquet, char * mess, const char * id, int num)
{
  char  n_id[TAILLE_ID + 1];
  char n_num[TAILLE_NUM_MESS + 1];
  char n_mess[TAILLE_MESSAGE + 1];
  format_id(n_id, id);
  format_num(n_num, num);
  format_mess(n_mess, mess);
  sprintf(paquet, "DIFF %s %s %s\r\n", n_num, n_id, n_mess);
}

void format_mess_desc(char * mess, char * desc)
{
  char n_desc[TAILLE_MESSAGE + 1];
  format_mess(n_desc, desc);
  sprintf(mess, "DESC %s \r\n", n_desc);
}

void format_mess_help(char * mess, char *desc_cmd , char * cmd)
{
  char n_desc[TAILLE_MESSAGE + 1];
  format_mess(n_desc, desc_cmd);
  sprintf(mess, "%s %s \r\n", cmd, n_desc);

}
