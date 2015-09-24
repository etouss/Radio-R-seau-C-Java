#include "email.h"

void send_email(char * tmp){
  struct addrinfo *first_info;
  struct addrinfo hints;
  struct sockaddr_in adress_sock;
  int retour = 0;
  extern const char * name_radio;
  //Init 0
  bzero(&hints,sizeof(struct addrinfo));
  //Set Filter IPV4 / TCP
  hints.ai_family = AF_INET;
  hints.ai_socktype=SOCK_STREAM;

  //On oubliera pas le free
  retour = getaddrinfo("nivose","smtp",&hints,&first_info);
  if(retour !=0){
    printf("%s\n",gai_strerror(retour));
    return ;
  }
  if(first_info!=NULL){
    adress_sock = *(struct sockaddr_in *)first_info->ai_addr;
  }
  //fin sans ip


  int desc_sock = socket(PF_INET,SOCK_STREAM,0);
  retour = connect(desc_sock,(struct sockaddr *)&adress_sock,sizeof(struct sockaddr_in));
  if(retour == -1){
    perror(strerror(errno));
    return ;
  }

  retour = send(desc_sock,"HELO nivose\r\n",strlen("HELO nivose\r\n"),0);
  if(retour == -1){
    perror(strerror(errno));
    //exit(1);
    return ;
  }
  char buff[1024];
  int size_rec=0;
  while((size_rec = recv(desc_sock,buff,1023*sizeof(char),0)) == 1023);

  retour = send(desc_sock,"MAIL FROM: toussain@informatique.univ-paris-diderot.fr\r\n",strlen("MAIL FROM: toussain@informatique.univ-paris-diderot.fr\r\n"),0);
  if(retour == -1){
    perror(strerror(errno));
    //exit(1);
    return ;
  }
  while((size_rec = recv(desc_sock,buff,1023*sizeof(char),0)) == 1023);


  retour = send(desc_sock,"RCPT TO:",strlen("RCPT TO:"),0);
  if(retour == -1){
    perror(strerror(errno));
    //exit(1);
    return ;
  }
  retour = send(desc_sock,tmp,strlen(tmp),0);
  if(retour == -1){
    perror(strerror(errno));
    //exit(1);
    return ;
  }
   retour = send(desc_sock,"\r\n",strlen("\r\n"),0);
  if(retour == -1){
    perror(strerror(errno));
    //exit(1);
    return ;
  }


  while((size_rec = recv(desc_sock,buff,1023*sizeof(char),0)) == 1023);
  buff[size_rec] = 0;
  if(strstr(buff,"Recipient ok") == NULL)return;

  retour = send(desc_sock,"DATA\r\n",strlen("DATA\r\n"),0);
  if(retour == -1){
    perror(strerror(errno));
    //exit(1);
    return ;
  }

  retour = send(desc_sock,"Subject: Radio En ligne\r\n\r\n",strlen("Subject: Radio En ligne\r\n\r\n"),0);
  if(retour == -1){
    perror(strerror(errno));
    //exit(1);
    return ;
  }

  retour = send(desc_sock,name_radio,strlen(name_radio),0);
  if(retour == -1){
    perror(strerror(errno));
    //exit(1);
    return ;
  }


  retour = send(desc_sock," en ligne\r\n",strlen(" en ligne\r\n"),0);
  if(retour == -1){
    perror(strerror(errno));
    //exit(1);
    return ;
  }
  retour = send(desc_sock,".\r\n",strlen(".\r\n"),0);
  if(retour == -1){
    perror(strerror(errno));
    //exit(1);
    return ;
  }
  retour = send(desc_sock,"QUIT\r\n",strlen("QUIT\r\n"),0);
  if(retour == -1){
    perror(strerror(errno));
    //exit(1);
    return ;
  }

  //Les free
  close(desc_sock);
  freeaddrinfo(first_info);
  return ;
 
}

void add_subscriber(char * email)
{
  extern const char * name_radio;
  int fic = 0;
  if ((fic = open(name_radio, O_WRONLY | O_CREAT | O_APPEND, 0700)) == -1)
  {
    printf("Probleme de subscribe\n");
    return;
  }
  char w[143];
  memset(w,'#',142);
  w[140]= '\r';
  w[141] = '\n';
  w[142]= 0;
  sprintf(w,"%s",email);
  w[strlen(w)]= '#';
  write(fic, w, strlen(w));
  close(fic);
}
void * warn_subscriber(void * null)
{
  extern const char * name_radio;
  int fic = 0;
  char tampon[145];
  int rc = 0;
  if ((fic = open(name_radio, O_RDONLY)) == -1)
  {
    printf("Probleme de subscribe\n");
    return NULL;
  }
  while((rc = read(fic,tampon,142)) != 0 ){
    tampon[rc] = 0;
    char * tmp = strchr(tampon,'#');
    tmp[0]=0;
    send_email(tampon);
  }
  return NULL;
  
}
