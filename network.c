#include "network.h"
#include "interface.h"
#include "structs_n_main.h"
#include <fcntl.h>
//debug
#include <stdio.h>
#define PORT "50000"
void startup(int argc, char* argv[], all_info *server){

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
}

int add_fd(fd_set* read_set, fd_set* write_set, ringfd active_fd){
  int max_fd = 0;

  FD_ZERO(read_set);
  FD_SET(STDIN_FILENO, read_set);
  if(active_fd.listen){
    FD_SET(active_fd.listen, read_set);
    FD_SET(active_fd.listen, write_set);
    max_fd = max(max_fd,active_fd.listen);
  }
  if (active_fd.udp){
    FD_SET(active_fd.udp, read_set);
    FD_SET(active_fd.udp, write_set);
    max_fd = max(max_fd,active_fd.udp);
  }
  if(active_fd.next){
    FD_SET(active_fd.next, read_set);
    FD_SET(active_fd.next, write_set);
    max_fd = max(max_fd,active_fd.next);
  }
  if(active_fd.prev){
    FD_SET(active_fd.prev, read_set);
    FD_SET(active_fd.prev, write_set);
    max_fd = max(max_fd,active_fd.prev);
  }
  return max_fd;
}


int init_UDPsv(all_info* server){
  int sockfd,n;
  struct addrinfo hints, *res;
  sockfd = socket(AF_INET, SOCK_DGRAM, 0); //UDP SOCKET
  fcntl(sockfd, F_SETFL, O_NONBLOCK);
  memset(&hints,0,sizeof hints);
  hints.ai_family=AF_INET;
  hints.ai_socktype=SOCK_DGRAM;
  hints.ai_flags=AI_PASSIVE;
  //1st parameter "127.0.0.1" for loopback,change to IP_PORT[1] for network use
  getaddrinfo(server->Myinfo.IP, server->Myinfo.port, &hints, &res);
  //binded
  n=bind(sockfd, res->ai_addr, res->ai_addrlen);
  if (n==-1)
    exit(1);
  freeaddrinfo(res);
  return sockfd;
}

int init_TCPsv(all_info* server)
{
  int fd,errcode;
  struct addrinfo hints,*res;
  fd=socket(AF_INET,SOCK_STREAM,0);
  fcntl(fd, F_SETFL, O_NONBLOCK);

  if (fd==-1)
    exit(1); //error

  memset(&hints,0,sizeof hints);
  hints.ai_family=AF_INET;//IPv4
  hints.ai_socktype=SOCK_STREAM;//TCP socket
  hints.ai_protocol=IPPROTO_TCP;//TCP socket

  errcode=getaddrinfo (server->Myinfo.IP,server->Myinfo.port,&hints,&res);
  if(errcode!=0)
    exit(1);//error

  if(bind(fd,res->ai_addr,res->ai_addrlen)!=0)
    exit(0);

  listen(fd, 5);
  freeaddrinfo(res);
  return fd;

}

void init_UDPcl(all_info* sv_info){
  int fd, errcode;
  struct addrinfo hints, *res;

  fd=socket(AF_INET, SOCK_DGRAM, 0);
  fcntl(fd, F_SETFL, O_NONBLOCK);
  if(fd==-1)
    exit(1);
  memset(&hints,0,sizeof hints);
  hints.ai_family=AF_INET;
  hints.ai_socktype=SOCK_DGRAM;

  errcode=getaddrinfo(sv_info->Next_info.IP, sv_info->Next_info.port, &hints, &res);
  if(errcode!=0)/*error*/
  exit(1);
  //sendto(fd,msg,42,0, res->ai_addr, res->ai_addrlen);
  //recvfrom();
  freeaddrinfo(res);
  close(fd);
}


int init_TCPcl(all_info* sv_info){
  int fd,errcode;
  ssize_t n;
  struct addrinfo hints,*res;

  fd=socket(AF_INET,SOCK_STREAM,0);
  fcntl(fd, F_SETFL, O_NONBLOCK);

  if (fd==-1)
    exit(1); //error

  memset(&hints,0,sizeof hints);
  hints.ai_family=AF_INET;//TCP socket
  hints.ai_socktype=SOCK_STREAM;//IPv4
  hints.ai_protocol=IPPROTO_TCP;//TCP socket

  errcode= getaddrinfo (sv_info->Next_info.IP, sv_info->Next_info.port , &hints, &res);
  if(errcode!=0)
    exit(1);//error

  n= connect (fd,res->ai_addr,res->ai_addrlen);

  if(n==-1){
    printf("could not connect\n");
    exit(1);//error
  }
  freeaddrinfo(res);
  return fd;
}

void send_request(int fd, const char* msg){
  int n;
  n=write(fd, msg, 9);
  if(n==-1){
    printf("write error\n");
    exit(1);//error
  }
}

int get_incoming(int fd){
  int fd_aux;

  if((fd_aux = accept(fd, NULL, NULL)) != -1)
    return fd_aux;
  else
    return 0;
}

int get_message(int fd, int prev){
  char buffer[20];
  size_t nbytes = sizeof(buffer);
  ssize_t bytes_read;

  bytes_read = read(fd, buffer, nbytes);
  
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