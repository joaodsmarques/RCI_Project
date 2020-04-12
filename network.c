#include "network.h"
#include "interface.h"
#include "structs_n_main.h"
#include <fcntl.h>
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

void setnonblocking(int sock){
  int opts;
  opts = fcntl(sock,F_GETFL);
  if(opts < 0)
    exit(1);
  opts = (opts | O_NONBLOCK);
  if(fcntl(sock,F_SETFL,opts)<0)
    exit(1);
}

int add_read_fd(fd_set* read_set, ringfd active_fd){
  int max_fd = 0;

  FD_ZERO(read_set);
  FD_SET(STDIN_FILENO, read_set);
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

int add_write_fd(int pending, fd_set* write_set, int fd, int max_fd){
  FD_ZERO(write_set);
  if(pending){
    FD_SET(fd, write_set);
    // printf("added to write_set\n");
    return max(max_fd,fd);
  }
  //printf("removed from write_set\n");
  return max_fd;
}


int init_UDPsv(all_info* server){
  int sockfd,n;
  int reuse_addr = 1;
  struct addrinfo hints, *res;
  sockfd = socket(AF_INET, SOCK_DGRAM, 0); //UDP SOCKET
  setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &reuse_addr, sizeof(reuse_addr));

  setnonblocking(sockfd);
  memset(&hints,0,sizeof hints);
  hints.ai_family=AF_INET;
  hints.ai_socktype=SOCK_DGRAM;
  hints.ai_flags=AI_PASSIVE;
  //1st parameter "127.0.0.1" for loopback,change to IP_PORT[1] for network use
  getaddrinfo(server->Myinfo.IP, server->Myinfo.port, &hints, &res);
  //binded
  n=bind(sockfd, res->ai_addr, res->ai_addrlen);
  if (n==-1){
    printf("error UDP\n");
    exit(1);
  }
  freeaddrinfo(res);
  return sockfd;
}

int init_TCPsv(all_info* server)
{
  int fd,errcode;
  int reuse_addr = 1;
  struct addrinfo hints,*res;
  fd=socket(AF_INET,SOCK_STREAM,0);
  setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, &reuse_addr, sizeof(reuse_addr));
  setnonblocking(fd);

  if (fd==-1){
    printf("error TCPsv\n");
    exit(1); //error
  }

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
  int reuse_addr = 1;
  struct addrinfo hints, *res;

  fd=socket(AF_INET, SOCK_DGRAM, 0);
  setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, &reuse_addr, sizeof(reuse_addr));
  setnonblocking(fd);
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
  int reuse_addr = 1;
  
  struct addrinfo hints,*res;

  fd=socket(AF_INET,SOCK_STREAM,0);
  setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, &reuse_addr, sizeof(reuse_addr));
  setnonblocking(fd);

  if (fd==-1){
    exit(1); //error
  }
  memset(&hints,0,sizeof hints);
  hints.ai_family=AF_INET;//TCP socket
  hints.ai_socktype=SOCK_STREAM;//IPv4
  hints.ai_protocol=IPPROTO_TCP;//TCP socket

  errcode= getaddrinfo (sv_info->Next_info.IP, sv_info->Next_info.port, &hints, &res);
  if(errcode!=0){
    exit(1);//error
  }
  connect(fd,res->ai_addr,res->ai_addrlen);
  printf("trying to connect\n");

  /*
  if(n==-1){
    printf("erro connect\n");
    exit(1);  
  }*/
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
  int reuse_addr = 1;

  if((fd_aux = accept(fd, NULL, NULL)) != -1){
    setsockopt(fd_aux, SOL_SOCKET, SO_REUSEADDR, &reuse_addr, sizeof(reuse_addr));
    setnonblocking(fd_aux);
    printf("connected\n");
    return fd_aux;
  }
  else
    return 0;
}

int get_message(int fd, char* msg){
  char* buffer=NULL;
  buffer = (char*) malloc(sizeof(char) * 50);
  size_t nbytes = (sizeof(char)*50);
  ssize_t bytes_read;
  memset(buffer,'\0',50);
  memset(msg,'\0',50);
  
  bytes_read = read(fd, buffer, nbytes);
  if(bytes_read == -1){
    printf("didnt read shit, erro\n");
    free(buffer);
    close(fd);
    return 0;
  }

  else if(bytes_read == 0){
    printf("n ha nada para ler porra\n");
    free(buffer);
    return 0;
  }
  else{
    strcpy(msg, strtok(buffer,"\n"));
    printf("read: %s\n", msg);
    free(buffer);
    return 1;  
  }
}

int isAlive(int fd){

  char buff[50];

    if(read(fd,buff,sizeof(buff)) <= 0){
      printf("IS DEAD\n");
      return 0;
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