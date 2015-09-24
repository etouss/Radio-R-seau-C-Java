#include "diffuseur.h"

const char * name_radio;

int safe_check_port(const char * to_convert){
   char *ptr;
   long ret;
   ret = strtol(to_convert, &ptr, 10);
   if(ptr != to_convert+strlen(to_convert))
     return 1;
   else if(ret>9999)
     return 1;
   return 0;
}

int main(int argc, char const *argv[])
{
    /*
       Check sur les arguments de la fonction.
    */

    int retour = 0;

    if(argc < 7)
        errx(1,"usage : diffuseur <nom diff> <port_diff_tcp> <address_diff_multi> <port_diff_multi> <addresse gestionnaire> <port gestionnaire>\n");
    const char * nom = argv[1];
    if(strlen(nom)>8)errx(1,"le nom du diffuseur doit être au plus de 8 caractere \n");
    const char * port_diffuseur_tcp = argv[2];
    if(safe_check_port(port_diffuseur_tcp))errx(1,"le port du diffuseur doit être au plus 9999 \n");
    const char * addresse_diffuseur_multi = argv[3];
    const char * port_diffuseur_multi = argv[4];
    if(safe_check_port(port_diffuseur_multi))errx(1,"le port du diffuseur doit être au plus 9999 \n");
    const char * addresse_gestionnaire = argv[5];
    const char * port_gestionnaire = argv[6];
    if(safe_check_port(port_gestionnaire))errx(1,"le port du gestionnaire doit être au plus 9999 \n"); 
    /*
      Variable permetant le check du bon déroulement des appelle syteme / reseau
    */
    //Init les differente radio possible !!
    name_radio = nom;
    init_config_radio();
    /*
       On init la connexion TCP on attend de pouoir creer son serveur:
       Puis on delegue la gestion de la communication a une thread separé.
       */
    int* sock_gestionnaire = init_radio(addresse_gestionnaire,port_gestionnaire,port_diffuseur_tcp,
                                        addresse_diffuseur_multi,port_diffuseur_multi,nom);
    if(sock_gestionnaire == NULL)
        {
            //Connection a la radio impossible !
            errx(1,"Connexion au gestionnaire impossible \n");
        }

    #ifdef DEBUG
    printf("Appairage avec le gestionnaire reussi \n");
    #endif

    pthread_t th_radio;
    void * param_diff[2];
    int indice_fonction_diff = NUMERO_RADIO;
    param_diff[0]=(void *)sock_gestionnaire;
    param_diff[1]=(void *)&indice_fonction_diff;
    retour = pthread_create(&th_radio,NULL,communication_radio_thread,param_diff);
    if(retour != 0)
        {
            perror(strerror(errno));
            close(*sock_gestionnaire);
            free(sock_gestionnaire);
        }
    /*
       On commence par ouvrir le serveur TCP
       */
    pthread_t th_client;
    void * param[2];
    int indice_fonction_input = NUMERO_RADIO;
    param[0]=(void *)port_diffuseur_tcp;
    param[1]=(void *)&indice_fonction_input;
    retour = pthread_create(&th_client,NULL,init_client_thread,param);
    if(retour != 0)
        {
            perror(strerror(errno));
        }
    #ifdef DEBUG
    printf("Creation du serveur TCP client reussi \n");
    #endif
    warn_subscriber(NULL);
    /*
       Puis on run la radio sur le thread principale.
    */
    run_radio(addresse_diffuseur_multi,port_diffuseur_multi,nom,NUMERO_RADIO);

    return 0;
}
