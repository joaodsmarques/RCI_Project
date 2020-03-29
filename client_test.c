#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
//Retirar este quando juntar ao Programa
#include <stdio.h>
#include <string.h>
#define PORT "2000"




int main(void)
{
  int fd,errcode;
  ssize_t n;
  socklen_t addrlen;
  struct addrinfo hints,*res;
  struct sockaddr_in addr;
  char buffer[128];
  const char *hostIP = "127.0.0.1";

  fd=socket(AF_INET,SOCK_DGRAM,0);
  if (fd==-1) exit(1); //error
  memset(&hints,0,sizeof hints);
  hints.ai_family=AF_INET;
  hints.ai_socktype=SOCK_DGRAM;
//UDP socket
//IPv4
//UDP socket
errcode= getaddrinfo (NULL,"5000",&hints,&res);
if(errcode!=0)/*error*/
  printf("merdaaaaa\n");

printf("hao\n");

if(connect(fd, res->ai_addr,res->ai_addrlen))
  printf("n ligou");


n=sendto(fd,"Hello!\n",7,0,res->ai_addr,res->ai_addrlen);




freeaddrinfo(res);
close (fd);





}
