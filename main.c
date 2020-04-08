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
  char buff[42];
  struct timeval timeout;
  ringfd active_fd;
  active_fd.prev=active_fd.next=active_fd.udp=active_fd.listen=active_fd.temp=0;
  fd_set read_set, write_set;
  all_info server;

  //Verifies input
  startup(argc, argv, &server);
  timeout.tv_sec = 2;
  timeout.tv_usec = 0;

  Display_menu();

  //Main program loop
  while(1){
    //will RESET and ADD all active FD to read_set using FD_ZERO and FD_SET
  	maxfd = add_fd(&read_set, &write_set, active_fd);

  	select(maxfd+1, &read_set, &write_set, (fd_set*) NULL, &timeout);
	 	//For user input
		if(FD_ISSET(STDIN_FILENO, &read_set)){
			switch (get_option()){
      //NEW i
      /*Be the 1st server in a new ring with key "i"*/
    		case 1:
      		if(!server.inRing){
        		server.key = new_i();
  					active_fd.udp = init_UDPsv(&server);
            active_fd.listen = init_TCPsv(&server);
            server.inRing = true;
      		}
          clrscreen();
          Display_menu();
    		break;
    		case 2:  //ENTRY i
    		break;
    		case 3:
          if(!server.inRing){
            sentry(&server);
            active_fd.udp = init_UDPsv(&server);
            active_fd.listen = init_TCPsv(&server);
            active_fd.next = init_TCPcl(&server); //connects to successor
            send_request(active_fd.next, "SUCCONF\n");
            server.inRing = true;
          }
            clrscreen();
            Display_menu();
    		break;
    		case 4:
    		break;
    		case 5: //SHOW
          show(server);
    		break;
    		case 6:
    		break;
    		case 7:
          exit(0);
    		break;
        default:
        clrscreen();
        Display_menu();         
  		}
		}
		//For UDP message received
		if(server.inRing && FD_ISSET(active_fd.udp, &read_set))
		{
			//usado para testar, dps vai fora
			recvfrom(active_fd.udp, buff, 42, 0, NULL, NULL);
			printf("%s\n", buff);
		}

    //New tcp connection incoming
    if(server.inRing && FD_ISSET(active_fd.listen, &read_set))
    {
      active_fd.temp = get_incoming(active_fd.listen);
    }

    //Will dealing with new tcp message
    if(server.inRing && active_fd.temp && FD_ISSET(active_fd.temp, &read_set))
    {
      get_message(active_fd.temp, active_fd.prev);
    }





	}
  return 0;
}
