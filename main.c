/*
Programa elaborado para a disciplina XXXX

Criadores:
Guilherme Guerreiro
João Marques

Explicação
Obrigado!
*/
#include "structs_n_main.h"
#include "interface.h"
#include "network.h"
#include <unistd.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <stdbool.h>

//for debug
#include <stdio.h>







int main(int argc, char* argv[])
{
  int maxfd = 0;
  int fd_udp, fd_tcp_next, fd_tcp_prev;
  char buff[10];
  struct timeval* timeout;
  fd_set sock_set;
  all_info server;


  //Verifies input
  startup(argc, argv, &server);
  
  timeout = NULL;

  Display_menu();

  //Main program loop :D
  while(1){
  	FD_ZERO(&sock_set);//needs to be reset every iteration
  	FD_SET(STDIN_FILENO, &sock_set);
  	maxfd = max(maxfd, STDIN_FILENO);

  	//only after being in the ring udp fd is set
  	if(server.inRing == true){ 
  		FD_SET(fd_udp, &sock_set);
  		maxfd = max(maxfd, fd_udp);
  	}

  	//FD_SET TCP
  	//FD_SET TCP
  	select(maxfd+1, &sock_set, (fd_set*) NULL, (fd_set*) NULL, timeout);

	 	//For user input
		if(FD_ISSET(STDIN_FILENO, &sock_set)){
			switch (get_option()){
      //NEW i
      /*Be the 1st server in a new ring with key "i"*/
    		case 1:
      		if(server.inRing == false){
        		server.key = new_i();
        		server.inRing = true;
        		  //Inicia servidor udp
  					fd_udp=init_UDPsv(&server);
      		}
      		else      
        		print("Server already in ring\nChoose another option\n");
    		break;
    		case 2:  //ENTRY i
    		break;
    		case 3:
    		break;
    		case 4:
    		break;
    		case 5:
    		break;
    		case 6:
    		break;
    		case 7:
    		break;
  		}
		}

		//For UDP message received
		if (FD_ISSET(fd_udp, &sock_set))
		{
			//usado para testar, dps vai fora
			recvfrom(fd_udp, buff, 7, 0, NULL, NULL);
			printf("%s\n", buff);
		}
	}
  return 0;
}
