#include "network.h"
#include "interface_n_aux.h"
#include "structs_n_main.h"

//Initializes the all node before starting the main cycle
//It could be written in the "interface_n_aux" file but as it is the
//beginning of the ring formation we decided to put it here
void startup(int argc, char* argv[], all_info *server, ringfd *active_fd){
  //We just have 3 entrance arguments
  if (argc != 3)
    exit(0);
    //Limits for the port
  else if(atoi(argv[2]) <= 1023 || atoi(argv[2]) > 64000)
  {
    print("The port must be a number between 1024 and 64000\n");
    exit(0);
  }//Minimal conditions for a valid IP: has a dot and must be shorter than IP_SIZE
  else if (strstr(argv[1],".")==NULL || strlen(argv[1]) >= IP_SIZE)
  {
    printf("Invalid IP\n");
    exit(0);
  }
  //In the beginning, the ring has one node so the successor and 2 successor is itself
  strcpy(server->Myinfo.IP, argv[1]);
  strcpy(server->Myinfo.port, argv[2]);
  strcpy(server->Next_info.IP, argv[1]);
  strcpy(server->Next_info.port, argv[2]);
  strcpy(server->SecondNext_info.IP, argv[1]);
  strcpy(server->SecondNext_info.port, argv[2]);
  server->key=-1;
  server->inRing = false;//we are not yet in the ring, the entrance option has to be chosen
  //Descriptors initializing
  active_fd->prev=active_fd->next=active_fd->udp=active_fd->listen=active_fd->temp=0;
}
//Set all the file descriptors that are ready to be set
//So the select function can deal with them
//Returns the maxfd of the file descriptors set
int add_read_fd(fd_set* read_set, ringfd active_fd){
  int max_fd = 0;
  //Needs to reset every iteration
  FD_ZERO(read_set);

  FD_SET(STDIN_FILENO, read_set);
  max_fd = max(max_fd, STDIN_FILENO);//Max between de new fd and maxfd
  //Set if they are being used
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

//Inits the udp file descriptor which will receive the udp messages
int init_UDPsv(all_info* _server){

  int sockfd,n,errcode;
  int reuse_addr = 1;
  struct addrinfo hints, *res;

  sockfd = socket(AF_INET, SOCK_DGRAM, 0); //UDP SOCKET
  if (sockfd==-1)
    exit(1); //error
  setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &reuse_addr, sizeof(reuse_addr));//To enable the reuse
  memset(&hints,0,sizeof hints);
  hints.ai_family=AF_INET; //IPV4
  hints.ai_socktype=SOCK_DGRAM;//TCP SOCKET
  hints.ai_flags=AI_PASSIVE;

  //Getting the information we need to bind
  errcode = getaddrinfo(NULL, _server->Myinfo.port, &hints, &res);

  if((errcode)!=0)
    exit(1); //error

  //binded
  n=bind(sockfd, res->ai_addr, res->ai_addrlen);
  if (n==-1)
    exit(1);

  freeaddrinfo(res);

  return sockfd;//Udp listenning is ready
}
//Inits a udp socket that will be used to send messages .
//Before we send a message we need to know where to send it
int init_UDPcl(all_info* server, struct addrinfo** udp_addr)
{
  int sockfd,errcode;
  int reuse_addr = 1;
  struct addrinfo hints;

  sockfd = socket(AF_INET, SOCK_DGRAM, 0); //UDP SOCKET
  if (sockfd==-1)
    exit(1); //error
  setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &reuse_addr, sizeof(reuse_addr));//enables the reuse
  memset(&hints,0,sizeof hints);
  hints.ai_family=AF_INET; //IPV4
  hints.ai_socktype=SOCK_DGRAM;//TCP SOCKET
  //Saving the info we need to send messages in udp_addr
  errcode = getaddrinfo(server->Next_info.IP, server->Next_info.port, &hints, udp_addr);
  if((errcode)!=0)
    exit(1); //error

  return sockfd;
}
//Inits a TCP socket in listen mode and returns the correspondig file descriptor
int init_TCP_Listen(all_info* _server)
{
  int errcode, newfd;
  int reuse_addr = 1;
  ssize_t n;
  struct addrinfo  hints;
  struct addrinfo * res;


  newfd=socket(AF_INET,SOCK_STREAM,0); //TCP socket

  if (newfd==-1)
    exit(1); //error

  memset(&hints, 0, sizeof hints);
  setsockopt(newfd, SOL_SOCKET, SO_REUSEADDR, &reuse_addr, sizeof(reuse_addr));

  hints.ai_family=AF_INET; //IPv4
  hints.ai_socktype=SOCK_STREAM; //TCP socket
  hints.ai_flags=AI_PASSIVE;//TCP Socket
  //Info we need to bind
  errcode = getaddrinfo(NULL,_server->Myinfo.port,&hints,&res);


  if((errcode)!=0)
    exit(1); //error

  //Binded
  n=bind(newfd,res->ai_addr,res->ai_addrlen);

  if(n==-1)
    exit(1);//error

  //Lets listen to a max of 6 waiting clients
  if(listen(newfd,6)==-1)
    exit(1); //error

  freeaddrinfo(res);

  return newfd;

}
//Inits a TCP socket and makes a connection from the function input
//Returns the correspondige descriptor
int init_TCP_connect(char* _IP, char* _port)
{
  int fd,errcode;
  int reuse_addr = 1;
  ssize_t n;
  struct addrinfo hints,*res;
  fd=socket(AF_INET,SOCK_STREAM,0);
  setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, &reuse_addr, sizeof(reuse_addr));


  if (fd==-1)
    exit(1); //error
  memset(&hints,0,sizeof hints);
  hints.ai_family=AF_INET;//TCP socket
  hints.ai_socktype=SOCK_STREAM;//IPv4
  hints.ai_protocol=IPPROTO_TCP;//TCP socket

  //Info from the input to be used to connect
  errcode= getaddrinfo (_IP, _port,&hints,&res);

  if(errcode!=0)
    exit(1);//error
  printf("waiting connection...\n");
  n= connect (fd,res->ai_addr,res->ai_addrlen);
  printf("got it!\n");

  if(n==-1)
    exit(1);//error

  freeaddrinfo(res);

  return fd;
}
//Used after the udpcl function. Sends the udp message
void send_udp(int fd, char* msg, struct sockaddr* addr, socklen_t addrlen){

  if(sendto(fd, msg, strlen(msg),0, addr, addrlen)==-1)
    exit(1);
}

//Receives a udp message.
void recv_udp(int fd, char* msg, struct sockaddr* addr, socklen_t* addrlen){

  if(recvfrom(fd, msg, 50, 0, addr, addrlen)==-1)
    exit(1);
}

//Sends a TCP message
void send_message(int fd, const char* msg){
  int n;
  n=write(fd, msg, strlen(msg));
  if(n==-1){
    printf("write error\n");
    exit(1);//error
  }
}

//Accepts the connection request if it is valid
//Returns:
//0:Reads null
//-1:Error reading
//1:Read did just fine
int get_incoming(int fd)
{
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
  {
    printf("Something went wrong while accepting a new connectio\n");
    exit(1);//non-valid connection request...
  }
}
//Reads the message
int get_message(int fd, char* msg){
  char* buffer=NULL;
  buffer = (char*) malloc(sizeof(char) * 50);
  size_t nbytes = (sizeof(char)*50);
  ssize_t bytes_read;
  memset(buffer,'\0',50);
  memset(msg,'\0',50);

  bytes_read = read(fd, buffer, nbytes);
  if(bytes_read == -1)
  {//Something went wrong
    printf("ERROR\n");
    free(buffer);
    close(fd);
    return -1;
  }
  //Reads Null, maybe someone left the ring
  else if(bytes_read == 0)
  {
    free(buffer);
    return 0;
  }
  else
  {
    //Everything is allright, lets see what we have here
    strcpy(msg, strtok(buffer,"\n"));//Copies all the message to a bufferand thamn to the msg spot
    strcat(msg,"\n");//adds the \n in the end of the message(terminator)
    free(buffer);
    return 1;
  }
}

//Close all the descriptors and inits the node so it's ready for another ring entrance
void close_all(ringfd* active_fd, all_info* server){

  //Closing all the descriptors
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

  //Resets the info as the server has never been in a ring before
  strcpy(server->Next_info.IP, server->Myinfo.IP);
  strcpy(server->Next_info.port, server->Myinfo.port);
  strcpy(server->SecondNext_info.IP, server->Myinfo.IP);
  strcpy(server->SecondNext_info.port, server->Myinfo.port);
  server->inRing = false;
  active_fd->prev=active_fd->next=active_fd->udp=active_fd->listen=active_fd->temp=0;
  server->key=server->succ_key=server->second_succ_key=-1;


}

//One of the main functions. Receives a formed message with the flag "FND"
//Analyses the message and discovers if needs to pass the mission of the
//Key find to the successor, can respond by itself or answer the client.
//Returns:
//0:If the mission was given to the succ or the mission is finished and
//We sent the answer to the client
//1:If there is 1 node in the ring
//2:If its successor has the key and it is the client who started the command
int Find_key(all_info myserver,char* msg, ringfd activefd)
{
  //Auxiliar to save the key we are searching
  int find_key=-1;
  char *aux;
  aux = (char*) malloc(sizeof(char) * 50);
  memset(aux,'\0',50);
  strcpy(aux,msg);//to dont destroy the received message
  char* IP;
  char* PORT;
  int newfd=0;
  int save_key=-1;
  //If I'm alone in the ring
  if(activefd.next==0 && activefd.prev==0)
  {
    clrscreen();
    printf("\n\n======================================||\n");
    printf("I HAVE THE KEY!\n");
    printf("Press Enter to return to Main Menu\n");
    printf("======================================||\n\n");
    return 1;
  }
  else//If the ring has at least 2 nodes
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
    else//My successor has the key
    {
      //Reads the client's key
      strtok(NULL," ");
      //Reads the client's IP and PORT
      IP=strtok(NULL," ");
      PORT=strtok(NULL,"\n");

      //If Im the client and my succ has the key im looking for
      if((strstr(myserver.Myinfo.IP,IP)!=NULL) && (strstr(myserver.Myinfo.port,PORT) != NULL))
      {
        //Saves the find_k in the secondsucckey temporarly and creates the message
        save_key=myserver.second_succ_key;
        myserver.second_succ_key=find_key;
        //clean aux
        memset(aux,'\0',50);
        //Creates the message to inform the client
        create_msg(aux, myserver, "KEY");
        //The key is replaced
        myserver.second_succ_key=save_key;
          Show_where_is_key(aux);
          return 2;
      }
      else//Connect to the client ant tells him where the key is
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
  return 0;
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
