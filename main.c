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
#include <signal.h>

//for debug
#include <stdio.h>



int main(int argc, char* argv[])
{
  int maxfd = 0;
  char buff[50];
  bool pending = false;
  //struct timeval timeout;
  ringfd active_fd;
  fd_set read_set, write_set;
  all_info server;
  struct sigaction act;
  memset(&act,0,sizeof act);
  act.sa_handler=SIG_IGN;
  sigaction(SIGPIPE,&act,NULL);

  //Verifies input
  startup(argc, argv, &server, &active_fd);
  Display_menu();

  //Main program loop
  while(1){
    //will RESET and ADD all active FD to read_set using FD_ZERO and FD_SET
  	maxfd = add_read_fd(&read_set, active_fd);
    maxfd = add_write_fd(pending, &write_set, active_fd.next, maxfd);
    //timeout.tv_sec = 5;
    //timeout.tv_usec = 0;

  	select(maxfd+1, &read_set, &write_set, (fd_set*) NULL, NULL);
	 	//For user input
		if(FD_ISSET(STDIN_FILENO, &read_set)){
			switch (get_option()){
      //NEW i
      /*Be the 1st server in a new ring with key "i"*/
    		case 1:
      		if(!server.inRing){
        		server.key = new_i();
            server.succ_key = server.key;
            server.second_succ_key = server.key;
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
            pending = true; //pending connection to be accepted by successor
            server.inRing = true;
          }
            clrscreen();
            Display_menu();
    		break;
    		case 4://LEAVE
        if(server.inRing){
          close(active_fd.next);
          close(active_fd.prev);
          close(active_fd.udp);
          close(active_fd.listen);
          close(active_fd.temp);
          server.inRing = false;
        }
    		break;
    		case 5: //SHOW
          show(server);
    		break;
    		case 6:
        isAlive(active_fd.prev, &read_set);
    		break;
    		case 7:
          exit(0);
    		break;
        default:
        clrscreen();
        Display_menu();         
  		}
		}

    /////////SERVER SIDE HANDLING////////
    /////////////////////////////////////
    /////////////////////////////////////

		//For UDP message received
    /*
		if(server.inRing && FD_ISSET(active_fd.udp, &read_set))
		{
			//usado para testar, dps vai fora
			recvfrom(active_fd.udp, buff, 42, 0, NULL, NULL);
			printf("%s\n", buff);
		}*/

    //New tcp connection incoming
    if(server.inRing && FD_ISSET(active_fd.listen, &read_set))
      active_fd.temp = get_incoming(active_fd.listen);

    //Will deal with new tcp message
    if(server.inRing && active_fd.temp && FD_ISSET(active_fd.temp, &read_set)){
      get_message(active_fd.temp, buff);

      //SUCCONF received
      if(message_analysis(buff, "SUCCONF")){
        //Answer with SUCC
          create_msg(buff, server, "SUCC");
          send_message(active_fd.temp, buff);
          //If there is only one server
          if(!active_fd.prev){
            active_fd.prev=active_fd.temp;
          }
      } 
      //NEW received
      else if(message_analysis(buff,"NEW")){
        //If there more than one server
        if(active_fd.prev != active_fd.temp){
          //Send info (of his new succ) to current prev, before switching*/
          send_message(active_fd.prev, buff);
          close(active_fd.prev);
          active_fd.prev = active_fd.temp;
          active_fd.temp = 0;
        }
        else{
        //If prev and temp are the same
        //It is the first connection in the ring, prev will also be the next
          parse_new(buff, &(server.SecondNext_info), &(server.second_succ_key));
          parse_new(buff, &(server.Next_info), &(server.succ_key));
          active_fd.next = init_TCPcl(&server);
          pending = true;
          active_fd.temp = 0;
        }
      }
      else{
        printf("unexpected message: abort\n");
        if(active_fd.prev == active_fd.temp)
          active_fd.prev = 0;
        close(active_fd.temp);
      }
    }

    //////////CLIENT SIDE HANDLING/////////
    ///////////////////////////////////////
    ///////////////////////////////////////

    //Wait until connection is accepted, then write
    if(FD_ISSET(active_fd.next, &write_set)){
      send_message(active_fd.next, "SUCCONF");
      pending = false;
    }
    //If connection is made and there is something to read
    if(active_fd.next && FD_ISSET(active_fd.next, &read_set)){

      if(!get_message(active_fd.next,buff)){

        strcpy(server.Next_info.port, server.SecondNext_info.port);
        server.succ_key=server.second_succ_key;
        active_fd.next = init_TCPcl(&server);
        pending = true;
        continue;

      }
      if(message_analysis(buff,"SUCC")){
        parse_new(buff, &(server.SecondNext_info), &(server.second_succ_key));
        //Give info to next, so next can inform his prev about me
        if(!active_fd.prev){
          create_msg(buff, server, "NEW");
          send_message(active_fd.next, buff);
        } 
      }
      else if(message_analysis(buff,"NEW")){
        parse_new(buff, &(server.Next_info), &(server.succ_key));
        create_msg(buff, server, "SUCC");
        send_message(active_fd.prev, buff);
        close(active_fd.next);
        active_fd.next = init_TCPcl(&server);
        pending = true;
      }
    }
	}
  return 0;
}
