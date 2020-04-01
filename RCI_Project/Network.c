
#include "Network.h"
#include "interface_n_aux.h"


all_info* NewServer_Heart(all_info* myserver)
{
  int maxfd = 0,newfd = 0;
  //Files to listen TCP and UDP communications
  int fd_udp=0,fdtcp_listen=0;
  //Files which connect to successor and predecessor
  int succ_fd=0,pre_fd=0;
  int n;
  //Time lapse variable
  struct timeval* timeout;
  timeout = NULL;

  struct sockaddr_in addr;
  socklen_t addrlen;

  //Flags
  int firstinit=-1, firstfriend=-1;

  fd_set sock_set;
  char buff[10];
  char buffer[VETOR_SIZE];

  //Time variable
  timeout = NULL;

  //New server main loop
  do
  {
    Display_new_menu();
    FD_ZERO(&sock_set);//needs to be reset every iteration

    //Initialize UDP and TCP listening file
    if(firstinit==-1)
    {
      fd_udp = init_UDPsv(myserver);
      fdtcp_listen = init_TCPsv(myserver);
      firstinit=0;
    }


    //Add stdin Inputs
    FD_SET(STDIN_FILENO, &sock_set);
    maxfd = max(maxfd, STDIN_FILENO);

    //Add udp
    FD_SET(fd_udp, &sock_set);
  	maxfd = max(maxfd, fd_udp);

    FD_SET(fdtcp_listen, &sock_set);
    maxfd = max(maxfd, fdtcp_listen);

    if(firstfriend==0)
    {
      FD_SET(pre_fd, &sock_set);
      maxfd = max(maxfd, pre_fd);

      FD_SET(succ_fd, &sock_set);
      maxfd = max(maxfd, succ_fd);

      //fazer flag para quando se volta a ter so um server que nao e o fundador
    }

    //Central Master controller
  	select(maxfd+1, &sock_set, (fd_set*) NULL, (fd_set*) NULL, timeout);

    //If the keyboard is pressed
    if(FD_ISSET(STDIN_FILENO, &sock_set))
    {

      switch (get_option_inserver())
      {
        case 1:
          printf("OPCAO 1\n");
    		break;

    		case 2:

        //Print the server state variables
        ServerState(myserver);
    		break;

    		case 3:
          printf("OPCAO 3\n");
    		break;

      }

    }
    //If there is a udp connection waiting
    if (FD_ISSET(fd_udp, &sock_set))
		{
      printf("cheguei fdps\n");
			//usado para testar, dps vai fora
			recvfrom(fd_udp, buff, 7, 0, NULL, NULL);
			printf("%s\n", buff);
		}

    //If there is someone waiting for a connection
    if (FD_ISSET(fdtcp_listen, &sock_set))
		{
      newfd = accept(fdtcp_listen,(struct sockaddr*)&addr,&addrlen);

      if(newfd==-1)
        exit(1); //error

      if(firstfriend==-1)
      {
        pre_fd = newfd;
        n = read (pre_fd,buffer,VETOR_SIZE);
        if(n==-1)
          exit(1);//error

        write(pre_fd,"FIRST\n",n);
        if(n==-1)
          exit(1);//Error

        printf("\n");
        myserver=FirstMessage_Analysis (buffer,myserver);
        succ_fd=TCP_InitnConect(myserver->Prev_info.IP, myserver->Prev_info.port);

        //close(pre_fd);
        //Nao esquecer FECHAR OS FICHEIROS A MAIS!!!!
        firstfriend=0;
      }

		}


  }while(myserver->inRing != false);

  return myserver;
}


all_info* SentryServer_Heart(all_info* myserver)
{
  int maxfd = 0,newfd = 0;
  //Files to listen TCP and UDP communications
  int fd_udp=0,fdtcp_listen=0;
  //Files which connect to successor and predecessor
  int succ_fd=0,pre_fd=0;
  ssize_t n;
  //Time lapse variable
  struct timeval* timeout;
  timeout = NULL;

  //to accept connections requests
  struct sockaddr_in addr;
  socklen_t addrlen;

  //flags
  //CRIAR FLAG PARA QUANDO VOLTAR A SER O UNICO SERVER RESTANTE
  int firstfriend=-1,init=-1;

  fd_set sock_set;
  char buff[10];
  char buffer[VETOR_SIZE];

  //Time variable
  timeout = NULL;

  fd_udp = init_UDPsv(myserver);
  fdtcp_listen = init_TCPsv(myserver);

  succ_fd= TCP_InitnConect( myserver->Next_info.IP, myserver->Next_info.port);

  //NEW 8 8.IP 8.TCP
  sprintf(buffer,"NEW %d %d.%s %d.%s\n", myserver->key, myserver->key,
  myserver->Myinfo.IP,myserver->key,myserver->Myinfo.port);

  printf("%s\n",buffer);
  write(succ_fd,buffer,n);

  //Succ server main loop
  do
  {
    Display_new_menu();
    FD_ZERO(&sock_set);//needs to be reset every iteration

    //Add stdin Inputs
    FD_SET(STDIN_FILENO, &sock_set);
    maxfd = max(maxfd, STDIN_FILENO);

    //Add udp
    FD_SET(fd_udp, &sock_set);
  	maxfd = max(maxfd, fd_udp);

    FD_SET(fdtcp_listen, &sock_set);
    maxfd = max(maxfd, fdtcp_listen);

    FD_SET(succ_fd, &sock_set);
    maxfd = max(maxfd, succ_fd);

    if(firstfriend==0)
    {
      FD_SET(pre_fd, &sock_set);
      maxfd = max(maxfd, pre_fd);

      //fazer flag para quando se volta a ter so um server que nao e o fundador
    }

    //Central Master controller
  	select(maxfd+1, &sock_set, (fd_set*) NULL, (fd_set*) NULL, timeout);

    //If the keyboard is pressed
    if(FD_ISSET(STDIN_FILENO, &sock_set))
    {

      switch (get_option_inserver())
      {
        case 1:
          printf("OPCAO 1\n");
    		break;

    		case 2:
        //Print the server state variables
        ServerState(myserver);
    		break;

    		case 3:
          printf("OPCAO 3\n");
    		break;

      }

    }
    //If there is a udp connection waiting
    if (FD_ISSET(fd_udp, &sock_set))
		{
      printf("cheguei fdps\n");
			//usado para testar, dps vai fora
			recvfrom(fd_udp, buff, 7, 0, NULL, NULL);
			printf("%s\n", buff);
		}

    //If there is someone waiting for a connection
    if (FD_ISSET(fdtcp_listen, &sock_set))
		{
      newfd = accept(fdtcp_listen,(struct sockaddr*)&addr,&addrlen);

      if(init==-1)
      {
        //Condição para quando entra no servidor
        pre_fd = newfd;
        n = read (pre_fd,buffer,VETOR_SIZE);
        if(n==-1)
          exit(1);//error
        //Copia o q interessa para o buffer

      }

		}

    if (FD_ISSET(succ_fd, &sock_set))
		{

    }


  }while(myserver->inRing != false);

}


int init_UDPsv (all_info* _server)
{
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

int init_TCPsv (all_info* _server)
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

//This function initializes a TCP socket and prepare a connection request
//It returns a File descriptor which indicates the established connection
int TCP_InitnConect( char IP[], char PORT[])
{
  int fd,errcode;
  ssize_t n;
  socklen_t addrlen;
  struct addrinfo hints,*res;
  struct sockaddr_in addr;
  char buffer[VETOR_SIZE];


  fd=socket(AF_INET,SOCK_STREAM,0);

  if (fd==-1)
    exit(1); //error

  memset(&hints,0,sizeof hints);
  hints.ai_family=AF_INET;//TCP socket
  hints.ai_socktype=SOCK_STREAM;//IPv4
  hints.ai_protocol=IPPROTO_TCP;//TCP socket

  errcode= getaddrinfo (IP,PORT,&hints,&res);

  if(errcode!=0)
    exit(1);//error


  n= connect (fd,res->ai_addr,res->ai_addrlen);

  if(n==-1)
    exit(1);//error

  return fd;

}




//
