
#include "Network.h"
#include "interface_n_aux.h"


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

int init_TCP_listen (char PORT[])
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

  errcode = getaddrinfo(NULL,PORT,&hints,&res);

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
  struct addrinfo hints,*res;



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

  freeaddrinfo(res);

  return fd;

}

/*type indicates if the server wars created by the sentry command or
the new i command.
type 0 = new
type 1 = entry

*/
all_info* Server_Heart(all_info* myserver, int type, ringfd mainfds)
{
  //Max nb in select, receiving filedescriptor and temporary fd
  int maxfd = 0,newfd = 0, auxfd=0;
  int n;

  //FLAG
  int firstfriend=-1;
  //Time lapse variable
  struct timeval timeout;
  timeout.tv_sec=2;
  timeout.tv_usec=0;

  struct sockaddr_in addr;
  socklen_t addrlen;

  fd_set sock_set;
  char buffer[VETOR_SIZE];
  char buffer2[VETOR_SIZE];
  char buff[10];

  mainfds.udp = init_UDPsv(myserver);

  if(type!=1)
    mainfds.listen = init_TCP_listen(myserver->Myinfo.port);

  //The entering server already has someone else in the ring
  if(type == 1)
    firstfriend = 1;

  do
  {
    Display_new_menu();

    //Timeout reset 1sec
    timeout.tv_sec=5;
    timeout.tv_usec=0;

    //needs to be reset every iterationallfds.listen = init_TCP_listen((*myserver)->Myinfo.port);
    FD_ZERO(&sock_set);

    //Add stdin Inputs
    FD_SET(STDIN_FILENO, &sock_set);
    maxfd = max(maxfd, STDIN_FILENO);

    //Add udp
    FD_SET(mainfds.udp, &sock_set);
  	maxfd = max(maxfd, mainfds.udp);

    FD_SET(mainfds.listen, &sock_set);
    maxfd = max(maxfd, mainfds.listen);

    if(firstfriend != -1 )
    {
      if(firstfriend == 0)
      {
        FD_SET(mainfds.pre, &sock_set);
        maxfd = max(maxfd, mainfds.pre);
      }

      FD_SET(mainfds.next, &sock_set);
      maxfd = max(maxfd, mainfds.next);
    }

    //Central Master controller
  	select(maxfd+1, &sock_set, (fd_set*) NULL, (fd_set*) NULL, &timeout);

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

    //UDP talking
    if (FD_ISSET(mainfds.udp, &sock_set))
		{
      recvfrom(mainfds.udp, buff, 7, 0, NULL, NULL);
			printf("%s\n", buff);
		}

    //Unknown talking
    if (FD_ISSET(mainfds.listen, &sock_set))
		{
      //Accept the new connection
      newfd = accept(mainfds.listen,(struct sockaddr*)&addr,&addrlen);

      if(newfd==-1)
        exit(1); //error

      n = read (newfd,buffer,VETOR_SIZE);

      if(n==-1)
      {
        printf("Read error\n");
        exit(1);//error
      }


      //Se for um pedido de entrada no anel
      if(strstr(buffer,"NEW")!=NULL)
      {
        //First connection of the new ring
        if(firstfriend==-1)
        {
          mainfds.pre=newfd;
          myserver = Message_Analysis (buffer,myserver,-1);
          //Excreve 2 sucessor no predecessor
          n=sprintf(buffer,"SUCC %d %d.%s %d.%s\n", myserver->succ_key, myserver->succ_key,
          myserver->Prev_info.IP,myserver->succ_key,myserver->Prev_info.port);
          printf("buffer:%s",buffer);

          write(mainfds.pre, buffer, n);
          if(n==-1)
          {
            printf("Read error\n");
            exit(1);//error
          }

          mainfds.next = TCP_InitnConect(myserver->Next_info.IP, myserver->Next_info.port);

          n=write(mainfds.next,"SUCCCONF\n", 9);
          if(n==-1)
          {
            printf("write error\n");
            exit(1);//error
          }

          n=write(mainfds.next,"FIRST\n", 6);
          if(n==-1)
          {
            printf("write error\n");
            exit(1);//error
          }

          firstfriend=0;
          //close(mainfds.pre);
          printf("FIZ TUDO\n");
        }
        else
        {
          //auxfd passa a ser o antigo predecessor
          auxfd = mainfds.pre;
          //novo predecessor e atualizado
          mainfds.pre = newfd;

          myserver = Message_Analysis (buffer,myserver,0);

          //Avisa o antigo predecessor do novo Sucessor
          n=write(auxfd, buffer, n);
          if(n==-1)
          {
            printf("write error\n");
            exit(1);//error
          }
          close(auxfd);

          //Diz qual o 2º sucessor do predecessor
          n=sprintf(buffer,"SUCC %d %d.%s %d.%s\n", myserver->succ_key, myserver->succ_key,
          myserver->Next_info.IP,myserver->succ_key,myserver->Next_info.port);

          n=write(mainfds.pre, buffer, n);
          if(n==-1)
          {
            printf("write error\n");
            exit(1);//error
          }

          //Mandar mensagem ao sucessor a perguntar se o sucessor dele se mantem ou nao!!!!(caso 2 servidores
          //e entra um novo- provoca alteracao do 2 sucessor)
        }
      }
      //SENTRY: Se for o pre do novo no do anel
      if(firstfriend == 1)
      {
        if(strstr(buffer,"SUCCCONF\n") !=NULL)
        {
          mainfds.pre=newfd;

          n = read (mainfds.pre,buffer,VETOR_SIZE);

          if(n==-1)
            exit(1);//error
          printf("JA estou ca\n");
          if(strstr(buffer,"FIRST\n")!=NULL)
          {
            strcpy(myserver->SecondNext_info.IP,myserver->Myinfo.IP);
            strcpy(myserver->SecondNext_info.port,myserver->Myinfo.port);
            strcpy(myserver->Prev_info.IP,myserver->Next_info.IP);
            strcpy(myserver->Prev_info.port,myserver->Next_info.port);

            firstfriend = 0;
          }
          if(strstr(buffer2,"SUCC")!=NULL)
          {
            myserver = Message_Analysis (buffer,myserver,2);
            firstfriend = 0;
          }
        }
      }
		}

    //Successor is talking
    if (FD_ISSET(mainfds.next, &sock_set))
		{

      //Recebe algo do sucessor
      n = read (mainfds.next,buffer, VETOR_SIZE);

      if(n==-1)
        exit(1);

      //eg: SUCC 12 12.IP 12.TCP - 2º Sucessor
      if(strstr(buffer,"SUCC")!=NULL)
        myserver=Message_Analysis(buffer, myserver,2);

      //Significa que vai ter um novo sucessor
      if(strstr(buffer,"NEW")!=NULL)
      {
        //Encerra sessão com antigo sucessor
        close(mainfds.next);
        myserver=Message_Analysis(buffer, myserver,1);
        //Nova sessão com novo sucessor
        mainfds.next = TCP_InitnConect(myserver->Next_info.IP, myserver->Next_info.port);

        n=write(mainfds.next,"SUCCCONF\n", 9);

        if(n==-1)
        {
          printf("write error\n");
          exit(1);//error
        }
        //Da ao novo sucessor as informações do predecessor (as suas)
        n=sprintf(buffer,"SUCC %d %d.%s %d.%s\n", myserver->key, myserver->key,
        myserver->Myinfo.IP,myserver->key,myserver->Myinfo.port);

        n=write(mainfds.next,buffer, n);

        if(n==-1)
        {
          printf("write error\n");
          exit(1);//error
        }
        //Diz ao predecessor que o segundo sucessor deste mudou
        n=sprintf(buffer,"SUCC %d %d.%s %d.%s\n", myserver->succ_key, myserver->succ_key,
        myserver->Next_info.IP,myserver->succ_key,myserver->Next_info.port);

        n=write(mainfds.pre,buffer,n);

        if(n==-1)
        {
          printf("write error\n");
          exit(1);//error
        }
      }
		}

    //Predecessor is talking
    if (FD_ISSET(mainfds.pre, &sock_set))
		{


		}

  }while(myserver->inRing != false);

  return myserver;
}

ringfd Sentry_Startup(all_info** myserver)
{
  char buffer[VETOR_SIZE];

  ringfd allfds;
  allfds.pre=0;
  allfds.listen=0;
  allfds.next=0;
  allfds.udp=0;

  int n;

  //Connect to the successor
  allfds.next = TCP_InitnConect( (*myserver)->Next_info.IP, (*myserver)->Next_info.port);
  allfds.listen = init_TCP_listen((*myserver)->Myinfo.port);

  //write ex: NEW 8 8.IP 8.TCP with his data
  n=sprintf(buffer,"NEW %d %d.%s %d.%s\n", (*myserver)->key, (*myserver)->key,
  (*myserver)->Myinfo.IP,(*myserver)->key,(*myserver)->Myinfo.port);



  write(allfds.next,buffer,n);
  if(n==-1)
  {
    printf("write error\n");
    exit(1);//error
  }

  //Expects to read the 2ºsucc information from the sucessor
  n = read (allfds.next,buffer,VETOR_SIZE);
  if(n==-1)
  {
    printf("Read error\n");
    exit(1);//error
  }

  //eg: SUCC 12 12.IP 12.TCP - 2º Sucessor
  if(strstr(buffer,"SUCC")!=NULL)
    (*myserver)=Message_Analysis(buffer, (*myserver),2);


  //FIRST CONNECTION OF THE PROCESS IS COMPLETE!


  //Accept the new connection- possibly the predecessor
  /*newfd = accept(allfds.listen,(struct sockaddr*)&addr,&addrlen);
  //MELHORAR ISTO PARA O CASO EM Q SE CONECTA UM SERVIDOR ALEATORIO Q NAO ERA SUPOST-retorno do listen??
  printf("ACEITO\n");
  if(newfd==-1)
    exit(1); //error

  n = read (newfd,buffer,sizeof(buffer));

  if(n==-1)
    exit(1);//error

  //Predecessor is identifying itself
  if(strstr(buffer,"SUCCCONF\n") !=NULL)
  {
    printf("JA estou ca\n");
    allfds.pre=newfd;
    n = read (allfds.pre,buffer,sizeof(buffer));

    if(strstr(buffer,"FIRST\n")!=NULL)
    {
      strcpy((*myserver)->SecondNext_info.IP,(*myserver)->Myinfo.IP);
      strcpy((*myserver)->SecondNext_info.port,(*myserver)->Myinfo.port);
      strcpy((*myserver)->Prev_info.IP,(*myserver)->Next_info.IP);
      strcpy((*myserver)->Prev_info.port,(*myserver)->Next_info.port);
    }
    if(strstr(buffer,"SUCC")!=NULL)
    {

    }
  }*/

  (*myserver)->inRing=true;

  printf("CHEGUEI!\n");
  return allfds;
}
