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




int main(int argc, char* argv[])
{
  int maxfd = 0;
  char buff[50];
  //struct timeval timeout;
  ringfd active_fd;
  fd_set read_set;
  all_info server;

  //Prevent SIGPIPE Signal
  struct sigaction act;
  memset(&act,0,sizeof act);
  act.sa_handler=SIG_IGN;
  sigaction(SIGPIPE,&act,NULL);

  //Verifies input
  startup(argc, argv, &server, &active_fd);
  Display_menu();

  //Main program loop
  while(1)
  {
    //will RESET and ADD all active FD to read_set using FD_ZERO and FD_SET
  	maxfd = add_read_fd(&read_set, active_fd);
    //timeout.tv_sec = 5;
    //timeout.tv_usec = 0;

  	select(maxfd+1, &read_set, (fd_set*) NULL, (fd_set*) NULL, NULL);
	 	//For user input
		if(FD_ISSET(STDIN_FILENO, &read_set))
    {
			switch (get_option())
      {
      //NEW i
      /*Be the 1st server in a new ring with key "i"*/
    		case 1:
      		if(!server.inRing)
          {
        		server.key = new_i();
            server.succ_key = server.key;
            server.second_succ_key = server.key;
  					active_fd.udp = init_UDPsv(&server);
            active_fd.listen = init_TCP_Listen(&server);
            active_fd.next=0;
            server.inRing = true;
      		}
          clrscreen();
          Display_menu();
    		break;
    		case 2:  //ENTRY i
        if(server.inRing)
          printf("You must leave the Ring First!!\n");
    		break;
    		case 3:
          if(!server.inRing)
          {
            sentry(&server);
            active_fd.udp = init_UDPsv(&server);
            active_fd.listen = init_TCP_Listen(&server);
            active_fd.next = init_TCP_connect(&server); //connects to successor
            //Sends the first message to the successor
            create_msg(buff, server, "NEW");
            send_message(active_fd.next, buff);
            server.inRing = true;
          }
            clrscreen();
            Display_menu();
    		break;
    		case 4://LEAVE
        if(server.inRing)
        {
          close(active_fd.next);
          close(active_fd.prev);
          close(active_fd.udp);
          close(active_fd.listen);
          close(active_fd.temp);
          active_fd.next=0;
          active_fd.prev=0;
          active_fd.udp=0;
          active_fd.listen=0;
          active_fd.temp=0;
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
          if(server.inRing)
            printf("You must leave the Ring First!!\n");
          else
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

    //Receinving a message from an Unknown node
    if(server.inRing && active_fd.temp && FD_ISSET(active_fd.temp, &read_set))
    {
      get_message(active_fd.temp, buff);
      //SUCCONF received
      if(strstr(buff,"SUCCCONF")!= NULL)
      {          //If there is only one-prev is 0
          if(active_fd.prev==0)
          {
            active_fd.prev=active_fd.temp;
          }
          else
          {
            //O servidor saiu. este será o nosso novo pre-por tudo pronto para tal
          }
      }
      //NEW received
      else if(strstr(buff,"NEW ") !=NULL)
      {
        //If there more than one server
        if(active_fd.next!=0)
        {
          //Send info (of his new succ) to current prev, before switching*/
          //Assume the new connection as predecessor and breaks up with the previous pred
          send_message(active_fd.prev, buff);
          close(active_fd.prev);
          active_fd.prev = active_fd.temp;
          active_fd.temp = 0;
          //Informs his new predecessor about his second successor
          create_msg(buff, server, "SUCC");
          send_message(active_fd.prev, buff);
        }
        //General case for at least 2 nodes in the ring
        else
        {
          //If prev and next are both 0
          //It is the first connection in the ring, prev will also be the next
          parse_new(buff, &(server.Next_info), &(server.succ_key));
          active_fd.prev = active_fd.temp;
          active_fd.temp = 0;
          //Informs the predecessor about his new second successor
          create_msg(buff, server, "SUCC");
          send_message(active_fd.prev, buff);

          active_fd.next = init_TCP_connect(&server);
          //Now the successor has to know his new predecessor
          send_message(active_fd.next,"SUCCCONF\n");
        }
      }
      else
      {
        printf("unexpected message: abort\n");
        if(active_fd.prev == active_fd.temp)
          active_fd.prev = 0;
        close(active_fd.temp);
      }
    }

    //////////CLIENT SIDE HANDLING/////////
    ///////////////////////////////////////
    ///////////////////////////////////////

    //If connection is made and there is something to read
    if(server.inRing && active_fd.next && FD_ISSET(active_fd.next, &read_set))
    {
      //If read returns 0, the connections was lost
      if(!get_message(active_fd.next,buff))
      {
        close(active_fd.next);
        printf("Vi te a sair Cafagestji\n");
        //CLEAN SECOND SUCC
        strcpy(server.Next_info.port, server.SecondNext_info.port);
        server.succ_key=server.second_succ_key;
        active_fd.next = init_TCP_connect(&server);
      }
      //Let's receive the Second successor
      if(strstr(buff,"SUCC ")!=NULL)
      {
        parse_new(buff, &(server.SecondNext_info), &(server.second_succ_key));
        //Give info to next, so next can inform his prev about me
      }
      else if(strstr(buff,"NEW ")!= NULL)
      {
        //Saving the new successor data
        strcpy(server.SecondNext_info.IP,server.Next_info.IP);
        strcpy(server.SecondNext_info.port,server.Next_info.port);
        server.second_succ_key=server.succ_key;
        parse_new(buff, &(server.Next_info), &(server.succ_key));
        //Send to the predecessor his second successor
        create_msg(buff, server, "SUCC");
        send_message(active_fd.prev, buff);
        close(active_fd.next);
        active_fd.next = init_TCP_connect(&server);
        send_message(active_fd.next,"SUCCCONF\n");
      }
    }
	}
  return 0;
}
