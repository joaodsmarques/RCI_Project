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
#define PORT "2200"


int main(void)
{
  int fd,errcode;
  ssize_t n;
  socklen_t addrlen;
  struct addrinfo hints,*res;
  struct sockaddr_in addr;
  char buffer[128];
  const char *hostIP = "127.0.0.1";


  fd=socket(AF_INET,SOCK_STREAM,0);
  if (fd==-1) exit(1); //error
  memset(&hints,0,sizeof hints);
  hints.ai_family=AF_INET;
  hints.ai_socktype=SOCK_STREAM;
  hints.ai_protocol=IPPROTO_TCP;
//TCP socket
//IPv4
//TCP socket
errcode= getaddrinfo (hostIP,PORT,&hints,&res);
if(errcode!=0)/*error*/
  exit(1);


n= connect (fd,res->ai_addr,res->ai_addrlen);
if(n==-1)/*error*/exit(1);


write (fd,"NEW 8 8.127.0.0.1 8.3300\n\0",128);
if(n==-1)/*error*/exit(1);

n= read (fd,buffer,128);

if(n==-1)//error
 exit(1);

/*write(1,"echo: ",6);
write(1,buffer,n);
*/


freeaddrinfo(res);
close (fd);





}
