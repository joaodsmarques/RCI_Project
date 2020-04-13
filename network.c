#include "network.h"
#include "interface.h"
#include "structs_n_main.h"
//debug
#include <stdio.h>
#include <strings.h>

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
  int reuse_addr = 1;
  struct addrinfo hints, *res;

  sockfd = socket(AF_INET, SOCK_DGRAM, 0); //UDP SOCKET
  if (sockfd==-1)
    exit(1); //error
  setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &reuse_addr, sizeof(reuse_addr));
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
  int reuse_addr =1;
  struct addrinfo  hints;
  struct addrinfo * res;


  newfd=socket(AF_INET,SOCK_STREAM,0); //TCP socket
  setsockopt(newfd, SOL_SOCKET, SO_REUSEADDR, &reuse_addr, sizeof(reuse_addr));

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

int init_TCP_connect(char* _IP, char* _port)
{
  int fd,errcode;
  int reuse_addr=1;
  ssize_t n;
  struct addrinfo hints,*res;
  fd=socket(AF_INET,SOCK_STREAM,0);

  if (fd==-1)
    exit(1); //error
  setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, &reuse_addr, sizeof(reuse_addr));
  memset(&hints,0,sizeof hints);
  hints.ai_family=AF_INET;//TCP socket
  hints.ai_socktype=SOCK_STREAM;//IPv4
  hints.ai_protocol=IPPROTO_TCP;//TCP socket

  errcode= getaddrinfo (_IP, _port,&hints,&res);

  if(errcode!=0)
    exit(1);//error
printf("waiting connection\n");
  n= connect (fd,res->ai_addr,res->ai_addrlen);
printf("got it\n");

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
  printf("sent: %s", msg);
}

int get_incoming(int fd){
  int fd_aux;
  int reuse_addr=1;
  struct sockaddr_in addr;
  socklen_t addrlen;

  if((fd_aux = accept(fd,(struct sockaddr*)&addr,&addrlen)) != -1)
  {
    setsockopt(fd_aux, SOL_SOCKET, SO_REUSEADDR, &reuse_addr, sizeof(reuse_addr));
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


void close_all(ringfd* active_fd, all_info* server){


  if(active_fd->next)
    close(active_fd->next);
  if (active_fd->prev)
    close(active_fd->prev);
  if(active_fd->prev)
    close(active_fd->udp);
  if(active_fd->listen)
    close(active_fd->listen);
  if(active_fd->temp)
    close(active_fd->temp);

  strcpy(server->Next_info.IP, server->Myinfo.IP);
  strcpy(server->Next_info.port, server->Myinfo.port);
  strcpy(server->SecondNext_info.IP, server->Myinfo.IP);
  strcpy(server->SecondNext_info.port, server->Myinfo.port);
  server->inRing = false;
  active_fd->prev=active_fd->next=active_fd->udp=active_fd->listen=active_fd->temp=0;
  server->key=server->succ_key=server->second_succ_key=-1;


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

void Find_key(all_info myserver,char* msg, ringfd activefd)
{

  int find_key=-1;
  char *aux;
  aux = (char*) malloc(sizeof(char) * 50);
  memset(aux,'\0',50);
  strcpy(aux,msg);
  char* IP;
  char* PORT;
  int newfd=0;
  int save_key=-1;

  if(activefd.next==0 && activefd.prev==0)
  {
    printf("I have the key!\n");
  }
  else
  {
    //Reads the message flag
    strtok(aux," ");
    //Saves the key we are searching
    find_key=atoi(strtok(NULL," "));

    //If the successor does not have the key, we give the mission to other
    if(Key_Distance(find_key, myserver.key,myserver.succ_key)==0)
    {
      send_message(activefd.next, msg);
    }
    else
    { printf("O meu next tem a chave\n");
      //Reads the client's key
      strtok(NULL," ");
      //Reads the client's IP and PORT
      IP=strtok(NULL," ");
      PORT=strtok(NULL,"\n");

      printf("ISTO %s com %s\nIst %s com %s\n",myserver.Next_info.IP,IP,myserver.Next_info.port,PORT);
      //If Im the client and my succ has the key im looking for
      if((strstr(myserver.Myinfo.IP,IP)!=NULL) && (strstr(myserver.Myinfo.port,PORT) != NULL))
      {
        //Saves the find_k in the secondsucckey temporarly and creates the message
        save_key=myserver.second_succ_key;
        myserver.second_succ_key=find_key;
        printf("%s %s %d\n",IP,PORT,myserver.second_succ_key);
        //clean aux
        memset(aux,'\0',50);
        //Creates the message to inform the client
        create_msg(aux, myserver, "KEY");
        //The key is replaced
        myserver.second_succ_key=save_key;
          Show_where_is_key(aux);
      }
      else
      {

        newfd=init_TCP_connect(IP,PORT);
        //Saves the find_k in the secondsucckey temporarly and creates the message
        save_key=myserver.second_succ_key;
        myserver.second_succ_key=find_key;
        printf("%s %s %d\n",IP,PORT,myserver.second_succ_key);
        //clean aux
        memset(aux,'\0',50);
        //Creates the message to inform the client
        create_msg(aux, myserver, "KEY");
        //The key is replaced
        myserver.second_succ_key=save_key;
        //Send the message to the cliente who asked
        send_message(newfd, aux);
        close(newfd);
      }
    }
  }
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
