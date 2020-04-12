#include "network.h"
#include "interface.h"
#include "structs_n_main.h"
//debug
#include <stdio.h>
#include <strings.h>
#define PORT "50000"
void startup(int argc, char* argv[], all_info *server, ringfd *active_fd){

  if (argc != 3)
    exit(0);

  else if(atoi(argv[2]) <= 1023 || atoi(argv[2]) > 64000)
  {
    print("The port must be a number between 1024 and 64000\n");
    exit(0);
  }
  strcpy(server->Myinfo.IP, argv[1]);
  strcpy(server->Myinfo.port, argv[2]);
  strcpy(server->Next_info.IP, argv[1]);
  strcpy(server->Next_info.port, argv[2]);
  strcpy(server->SecondNext_info.IP, argv[1]);
  strcpy(server->SecondNext_info.port, argv[2]);
  server->key=-1;
  server->inRing = false;
  active_fd->prev=active_fd->next=active_fd->udp=active_fd->listen=active_fd->temp=0;
}

int add_read_fd(fd_set* read_set, ringfd active_fd){
  int max_fd = 0;

  FD_ZERO(read_set);

  FD_SET(STDIN_FILENO, read_set);
  max_fd = max(max_fd, STDIN_FILENO);

  if(active_fd.listen){
    FD_SET(active_fd.listen, read_set);
    max_fd = max(max_fd,active_fd.listen);
  }
  if (active_fd.udp){
    FD_SET(active_fd.udp, read_set);
    max_fd = max(max_fd,active_fd.udp);
  }
  if(active_fd.next){
    FD_SET(active_fd.next, read_set);
    max_fd = max(max_fd,active_fd.next);
  }
  if(active_fd.prev){
    FD_SET(active_fd.prev, read_set);
    max_fd = max(max_fd,active_fd.prev);
  }
  if (active_fd.temp){
    FD_SET(active_fd.temp, read_set);
    max_fd = max(max_fd,active_fd.temp);
  }
  return max_fd;
}

//Pus a versao antiga disto
int init_UDPsv(all_info* _server){

  int sockfd,n,errcode;
  struct addrinfo hints, *res;

  sockfd = socket(AF_INET, SOCK_DGRAM, 0); //UDP SOCKET
  if (sockfd==-1)
    exit(1); //error

  memset(&hints,0,sizeof hints);
  hints.ai_family=AF_INET; //IPV4
  hints.ai_socktype=SOCK_DGRAM;//TCP SOCKET
  hints.ai_flags=AI_PASSIVE;


  errcode = getaddrinfo(NULL, _server->Myinfo.port, &hints, &res);

  if((errcode)!=0)
    exit(1); //error

  //binded
  n=bind(sockfd, res->ai_addr, res->ai_addrlen);
  if (n==-1)
    exit(1);

  freeaddrinfo(res);

  return sockfd;
}

int init_TCP_Listen(all_info* _server)
{
  int errcode, newfd;
  int n;
  struct addrinfo  hints;
  struct addrinfo * res;


  newfd=socket(AF_INET,SOCK_STREAM,0); //TCP socket

  if (newfd==-1)
    exit(1); //error

  memset(&hints, 0, sizeof hints);

  hints.ai_family=AF_INET; //IPv4
  hints.ai_socktype=SOCK_STREAM; //TCP socket
  hints.ai_flags=AI_PASSIVE;

  errcode = getaddrinfo(NULL,_server->Myinfo.port,&hints,&res);

  if((errcode)!=0)
    exit(1); //error

  //Binded
  n=bind(newfd,res->ai_addr,res->ai_addrlen);

  if(n==-1)
    exit(1);//error


  if(listen(newfd,6)==-1)
    exit(1); //error

  freeaddrinfo(res);

  return newfd;

}

int init_TCP_connect(all_info* sv_info)
{
  int fd,errcode;
  ssize_t n;
  struct addrinfo hints,*res;



  fd=socket(AF_INET,SOCK_STREAM,0);

  if (fd==-1)
    exit(1); //error

  memset(&hints,0,sizeof hints);
  hints.ai_family=AF_INET;//TCP socket
  hints.ai_socktype=SOCK_STREAM;//IPv4
  hints.ai_protocol=IPPROTO_TCP;//TCP socket

  errcode= getaddrinfo (sv_info->Next_info.IP, sv_info->Next_info.port,&hints,&res);

  if(errcode!=0)
    exit(1);//error


  n= connect (fd,res->ai_addr,res->ai_addrlen);

  if(n==-1)
    exit(1);//error

  freeaddrinfo(res);

  return fd;
}

void send_message(int fd, const char* msg){
  int n;
  n=write(fd, msg, strlen(msg));
  if(n==-1){
    printf("write error\n");
    exit(1);//error
  }
  printf("sent %s\n", msg);
}

int get_incoming(int fd){
  int fd_aux;
  struct sockaddr_in addr;
  socklen_t addrlen;

  if((fd_aux = accept(fd,(struct sockaddr*)&addr,&addrlen)) != -1)
  {
    printf("connected\n");
    return fd_aux;
  }
  else
    exit(1);
}

int get_message(int fd, char* msg){
  char* buffer=NULL;
  buffer = (char*) malloc(sizeof(char) * 50);
  size_t nbytes = (sizeof(char)*50);
  ssize_t bytes_read;
  memset(buffer,'\0',50);
  memset(msg,'\0',50);

  bytes_read = read(fd, buffer, nbytes);
  if(bytes_read == -1)
  {
    printf("didnt read,ERROR\n");
    free(buffer);
    close(fd);
    return 0;
  }

  else if(bytes_read == 0)
  {
    printf("Reading NULL (0 return)\n");
    free(buffer);
    return 0;
  }
  else
  {
    strcpy(msg, strtok(buffer,"\n"));
    printf("read: %s\n", msg);
    free(buffer);
    return 1;
  }
}

int isAlive(int fd, fd_set *read_set){

  char buff[50];

  if(FD_ISSET(fd, read_set)){
    if(read(fd,buff,sizeof(buff)) < 0){
      printf("IS DEAD\n");
      return 0;
    }
  }
  printf("is alive\n");
  return 1;
}





















































/*
void TCP_Server_Create(char * PORT, char * _IP)
{
  int fd,errcode, newfd;
  ssize_t n;
  socklen_t addrlen;
  struct addrinfo  hints;
  struct addrinfo * res;
  struct sockaddr_in addr;
  char buffer[128];


  fd=socket(AF_INET,SOCK_STREAM,0); //TCP socket
  if (fd==-1)
    exit(1); //error

  memset(&hints, 0, sizeof hints);
  hints.ai_family=AF_INET; //IPv4
  hints.ai_socktype=SOCK_STREAM; //TCP socket
  hints.ai_flags=AI_PASSIVE;


  errcode = getaddrinfo(NULL,PORT,&hints,&res);

  if((errcode)!=0)
    exit(1); //error

  if(bind(fd,res->ai_addr,res->ai_addrlen)==-1)
    exit(1);//error

  if(listen(fd,5)==-1)
    exit(1); //error
  printf("TCP server initialized!\n");


  while(1)
  {
    addrlen=sizeof(addr);

    if((newfd=accept(fd,(struct sockaddr*)&addr,&addrlen))==-1)
      exit(1); //error

    n = read (newfd,buffer,128);
    if(n==-1)
      exit(1); //error

    write(1,"received: ",10);
    write(1,buffer,n);
    n=write(newfd,buffer,n);

    if(n==-1)
      exit(1); //error


    close(newfd);
  }

freeaddrinfo(res);
close (fd);
}*/
