
#include "Network.h"
#include "interface_n_aux.h"

/*void TCP_Server_Connect(Closet * Mystuff)
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

  errcode = getaddrinfo(NULL,Mystuff->Mypurse.port,&hints,&res);

  if((errcode)!=0)
    exit(1); //error

  n=bind(fd,res->ai_addr,res->ai_addrlen);

  if(n==-1)
    exit(1);//error

  if(listen(fd,5)==-1)
    exit(1); //error

  printf("Server listening...\n");

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

}

*/

all_info* NewServer_Heart(all_info* myserver)
{
  int maxfd = 0;
  int fd_udp=0;
  struct timeval* timeout;
  timeout = NULL;
  int firstime=1;

  fd_set sock_set;
  char buff[10];

  //Time variable
  timeout = NULL;

  //New server main loop
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

    //Initialize Udp
    if(firstime==1)
    {
      fd_udp=init_UDPsv(myserver);
      firstime=0;
    }
    //FD_SET TCP
  	//FD_SET TCP
  	select(maxfd+1, &sock_set, (fd_set*) NULL, (fd_set*) NULL, timeout);

    if(FD_ISSET(STDIN_FILENO, &sock_set))
    {

      switch (get_option_inserver())
      {
        case 1:
          printf("OPCAO 1\n");
    		break;

    		case 2:
          printf("OPCAO 2\n");
    		break;

    		case 3:
          printf("OPCAO 3\n");
    		break;

      }

    }

    if (FD_ISSET(fd_udp, &sock_set))
		{
        printf("cheguei fdps\n");
			//usado para testar, dps vai fora
			recvfrom(fd_udp, buff, 7, 0, NULL, NULL);
			printf("%s\n", buff);
		}


  }while(myserver->inRing != false);

  return myserver;
}

int init_UDPsv (all_info* _server)
{
  int sockfd,n;
  struct addrinfo hints, *res;

  sockfd = socket(AF_INET, SOCK_DGRAM, 0); //UDP SOCKET

  memset(&hints,0,sizeof hints);
  hints.ai_family=AF_INET;
  hints.ai_socktype=SOCK_DGRAM;
  hints.ai_flags=AI_PASSIVE;


  getaddrinfo(NULL, _server->Myinfo.port, &hints, &res);

  //binded
  n=bind(sockfd, res->ai_addr, res->ai_addrlen);
  if (n==-1)
    exit(1);

  freeaddrinfo(res);

  return sockfd;
}
