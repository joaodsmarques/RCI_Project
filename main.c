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
  int pending=0;
  int maxfd = 0;
  char buff[50];
  //struct timeval timeout;
  ringfd active_fd;
  active_fd.prev=active_fd.next=active_fd.udp=active_fd.listen=active_fd.temp=0;
  fd_set read_set, write_set;
  all_info server;

  //Verifies input
  startup(argc, argv, &server);
  Display_menu();
  FD_ZERO(&write_set);

  //Main program loop
  while(1){
    //will RESET and ADD all active FD to read_set using FD_ZERO and FD_SET
  	maxfd = add_read_fd(&read_set, active_fd);
    maxfd = add_write_fd(pending, &write_set, active_fd.next, maxfd);
    //timeout.tv_sec = 5;
    //timeout.tv_usec = 0;

  	select(maxfd+1, &read_set, &write_set, (fd_set*) NULL, NULL);
/*
if(FD_ISSET(active_fd.temp, &read_set))
  printf("well shit what now? 1\n");
if(FD_ISSET(active_fd.listen,&read_set))
  printf("well shit what now? 2\n");
if(FD_ISSET(active_fd.prev,&read_set))
  printf("well shit what now? 3\n");
if(FD_ISSET(active_fd.next,&read_set))
  printf("well shit what now? 4\n");

*/

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
            //add_write_fd(pending, &write_set, active_fd.next, maxfd);
            pending = true; //pending connection accepted
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
    if(server.inRing && FD_ISSET(active_fd.listen, &read_set)){
      active_fd.temp = get_incoming(active_fd.listen);
printf("get incoming\n");
    }

    //Will dealing with new tcp message
    if(server.inRing && active_fd.temp && FD_ISSET(active_fd.temp, &read_set)){
      get_message(active_fd.temp, buff);
      //SUCCONF received
      if(message_analysis(buff, "SUCCONF")){

          create_msg(buff, server, "SUCC");
          send_message(active_fd.temp, buff);
          if(!active_fd.prev){
            active_fd.prev=active_fd.temp;
          }

      } //Will receive the previous server data(antecessor)
      else if(message_analysis(buff,"NEW")){
        //Se houver prev
        if(active_fd.prev != active_fd.temp){
          /*Enviar ao atual prev as informaç~oes recebidas*/
          send_message(active_fd.prev, buff);
          close(active_fd.prev);
          active_fd.prev = active_fd.temp;
          active_fd.temp = 0;
          //pending = false; 
        }
        else{
        //se n~ao houver, apenas se atualiza o fd para o prev
        //Next e prev s~ao o mm servidor. ´e preciso ligar tcp
          parse_new(buff, &(server.SecondNext_info), &(server.second_succ_key));
          parse_new(buff, &(server.Next_info), &(server.succ_key));
          active_fd.next = init_TCPcl(&server);
          pending = true;
          active_fd.temp = 0;
        }
   
      }
      else{
        printf("unexpected message: abort\n");
        close(active_fd.temp);
      }
    }
    //////////CLIENT SIDE HANDLING/////////
    ///////////////////////////////////////
    ///////////////////////////////////////

    if(FD_ISSET(active_fd.next, &write_set)){
      send_message(active_fd.next, "SUCCONF");
      pending = false;
    }

    if(active_fd.next && FD_ISSET(active_fd.next, &read_set)){
      get_message(active_fd.next,buff);
      if(message_analysis(buff,"SUCC")){
        parse_new(buff, &(server.SecondNext_info), &(server.second_succ_key));
        printf("2succ %d, succ %d\n", server.second_succ_key , server.succ_key);
        if(!(active_fd.prev && active_fd.next)){
          create_msg(buff, server, "NEW");
          send_message(active_fd.next, buff);
        }
        
      }
      else if(message_analysis(buff,"NEW")){
        parse_new(buff, &(server.Next_info), &(server.succ_key));
        close(active_fd.next);
        active_fd.next = init_TCPcl(&server);
        pending = true;
      }
    }


	}
  return 0;
}
