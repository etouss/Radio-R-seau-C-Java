#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <errno.h>
#include <unistd.h>
#include <err.h>
#include <string.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netdb.h>
#include <fcntl.h>

void breakpoint(){};

int main(){
  struct addrinfo *first_info;
  struct addrinfo hints;
  struct sockaddr_in adress_sock;
  int retour = 0;
  //Init 0
  bzero(&hints,sizeof(struct addrinfo));
  //Set Filter IPV4 / TCP
  hints.ai_family = AF_INET;
  hints.ai_socktype=SOCK_STREAM;

  //On oubliera pas le free
  retour = getaddrinfo("nivose","smtp",&hints,&first_info);
  if(retour !=0){
    printf("%s\n",gai_strerror(retour));
    return 1;
  }
  if(first_info!=NULL){
    adress_sock = *(struct sockaddr_in *)first_info->ai_addr;
  }
  //fin sans ip

  breakpoint();

  int desc_sock = socket(PF_INET,SOCK_STREAM,0);
  retour = connect(desc_sock,(struct sockaddr *)&adress_sock,sizeof(struct sockaddr_in));
  if(retour == -1){
    perror(strerror(errno));
    return 1;
  }

  

  /*
     USAGE SAMPLE
     */
  /* char buff[1024]; */
  /* int size_rec=recv(desc_sock,buff,1023*sizeof(char),0); */
  /* buff[size_rec]='\0'; */
  /* char *mess="SALUT!\n"; */
  retour = send(desc_sock,"HELO nivose\r\n",strlen("HELO nivose\r\n"),0);
  if(retour == -1){
    perror(strerror(errno));
    //exit(1);
    return 1;
  }
  char buff[1024];
  int size_rec=0;
  while((size_rec = recv(desc_sock,buff,1023*sizeof(char),0)) == 1023){printf("%s",buff);fflush(stdout);}
  printf("%s",buff);fflush(stdout);

  retour = send(desc_sock,"MAIL FROM: toussain@informatique.univ-paris-diderot.fr\r\n",strlen("MAIL FROM: toussain@informatique.univ-paris-diderot.fr\r\n"),0);
  if(retour == -1){
    perror(strerror(errno));
    //exit(1);
    return 1;
  }
  while((size_rec = recv(desc_sock,buff,1023*sizeof(char),0)) == 1023){printf("%s",buff);fflush(stdout);}
  printf("%s",buff);fflush(stdout);

  retour = send(desc_sock,"RCPT TO: toussain\r\n",strlen("RCPT TO: toussain\r\n"),0);
  if(retour == -1){
    perror(strerror(errno));
    //exit(1);
    return 1;
  }
  while((size_rec = recv(desc_sock,buff,1023*sizeof(char),0)) == 1023){printf("%s",buff);fflush(stdout);}
  printf("%s",buff);fflush(stdout);
  retour = send(desc_sock,"DATA\r\n",strlen("DATA\r\n"),0);
  if(retour == -1){
    perror(strerror(errno));
    //exit(1);
    return 1;
  }
  retour = send(desc_sock,"Online : radio\r\n",strlen("Online : radio\r\n"),0);
  if(retour == -1){
    perror(strerror(errno));
    //exit(1);
    return 1;
  }
  retour = send(desc_sock,".\r\n",strlen(".\r\n"),0);
  if(retour == -1){
    perror(strerror(errno));
    //exit(1);
    return 1;
  }
  retour = send(desc_sock,"QUIT\r\n",strlen("QUIT\r\n"),0);
  if(retour == -1){
    perror(strerror(errno));
    //exit(1);
    return 1;
  }

  //Les free
  close(desc_sock);
  freeaddrinfo(first_info);
  return 0;
}
