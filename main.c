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
  int udp_key;
  char buff[50];
  struct addrinfo* udp_addr;
  struct sockaddr_in addr;
  socklen_t addrlen;
  struct timeval *timeout=NULL;
  struct timeval udp_timeout;
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
  	if(!select(maxfd+1, &read_set, (fd_set*) NULL, (fd_set*) NULL, timeout)){
      if(!server.inRing){
        //DEAL_WITH_IT();
        timeout = NULL;
      }
    }
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
            server.inRing = true;
      		}
          clrscreen();
          Display_menu();
    		break;
    		case 2:  //ENTRY i
        if(!server.inRing){
          entry_i(&server);
          active_fd.udp = init_UDPcl(&server, &udp_addr);
          create_msg(buff, server, "EFND");
          send_udp(active_fd.udp, buff, udp_addr->ai_addr, udp_addr->ai_addrlen);
          udp_timeout.tv_sec = 5;
          udp_timeout.tv_usec = 0;
          timeout = &udp_timeout;
        }

    		break;
    		case 3:
          if(!server.inRing)
          {
            sentry(&server);
            active_fd.udp = init_UDPsv(&server);
            active_fd.listen = init_TCP_Listen(&server);
            active_fd.next = init_TCP_connect(server.Next_info.IP,server.Next_info.port); //connects to successor
            //Sends the first message to the successor
            create_msg(buff, server, "NEW");
            send_message(active_fd.next, buff);
            server.inRing = true;
            clrscreen();
            Display_menu();
          }
          else
          {
            clrscreen();
            printf("You must leave the current ring first\n");
            Display_menu();
          }
    		break;
    		case 4://LEAVE
        if(server.inRing){
          close_all(&active_fd, &server);
          printf("%d\n", server.inRing);
        }
        clrscreen();
        Display_menu();
    		break;
    		case 5: //SHOW
          show(server);
    		break;
    		case 6:
          if(!server.inRing)
            printf("You must enter in a Ring First!!\n");
          else
          {
            Start_Search(buff,server);
            printf("Encontra:%s\n",buff);
            Find_key(server,buff, active_fd);
            memset(buff,'\0',50);
          }
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
		if(active_fd.udp && FD_ISSET(active_fd.udp, &read_set))
		{
      if(!server.inRing)
      {
        memset(buff,'\0',50);
        recvfrom(active_fd.udp, buff, 50, 0, NULL, NULL);
        printf("received udp %s\n", buff);
        timeout = NULL;
        close(active_fd.udp);
        if(parse_EKEY(buff, &server)==-1)
        {
          clrscreen();
          printf("--Key already taken, try again!--\n");
          printf("(Press Enter to go to Main Menu)\n");
          active_fd.udp=0;
        }
        else
        {
          active_fd.udp = init_UDPsv(&server);
          active_fd.listen = init_TCP_Listen(&server);
          active_fd.next = init_TCP_connect(server.Next_info.IP,server.Next_info.port); //connects to successor
          //Sends the first message to the successor
          create_msg(buff, server, "NEW");
          send_message(active_fd.next, buff);
          server.inRing = true;
        }
      }
      else
      {
        memset(buff,'\0',50);
        recvfrom(active_fd.udp, buff, 50, 0, (struct sockaddr*)&addr, &addrlen);
        printf("received udp: %s\n", buff);
        strtok(buff," ");
        sscanf(strtok(NULL,"\0"),"%d",&udp_key);
        memset(buff,'\0', 50);
        sprintf(buff,"FND %d %d %s %s\n",udp_key, server.key, server.Myinfo.IP, server.Myinfo.port);
        switch(Find_key(server, buff, active_fd)){
          case 1://a chave esta logo neste servidor
            memset(buff,'\0', 50);
            sprintf(buff,"EKEY %d %d %s %s",udp_key, server.key, server.Myinfo.IP, server.Myinfo.port);
            send_udp(active_fd.udp, buff, (struct sockaddr*)&addr, addrlen);
          break;
          case 2://a chave esta no sucessor
            memset(buff,'\0', 50);
            sprintf(buff,"EKEY %d %d %s %s",udp_key, server.succ_key, server.Next_info.IP, server.Next_info.port);
            send_udp(active_fd.udp, buff, (struct sockaddr*)&addr, addrlen);
          break;

        }
      }
		}

    //New tcp connection incoming
    if(server.inRing && FD_ISSET(active_fd.listen, &read_set))
      active_fd.temp = get_incoming(active_fd.listen);


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
        //Ver melhor condições de saída

        if(strstr(server.Next_info.port,server.Myinfo.port)!=NULL && strstr(server.Next_info.IP,server.Myinfo.IP)!=NULL)
        {
          active_fd.next=0;
        }
        else
        {
          printf("Foi onde nao querias\n");
          printf("%s %s %s %s\n",server.Next_info.port,server.Myinfo.port,server.Next_info.IP,server.Myinfo.IP);
          active_fd.next = init_TCP_connect(server.Next_info.IP,server.Next_info.port);
          create_msg(buff, server, "SUCC");
          send_message(active_fd.prev,buff);
          send_message(active_fd.next,"SUCCCONF\n");
        }

      }
      //Let's receive the Second successor
      if(strstr(buff,"SUCC ")!=NULL)
      {
        parse_new(buff, &(server.SecondNext_info), &(server.second_succ_key));
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
        active_fd.next = init_TCP_connect(server.Next_info.IP,server.Next_info.port);
        send_message(active_fd.next,"SUCCCONF\n");
      }
    }

    if(server.inRing && active_fd.prev && FD_ISSET(active_fd.prev, &read_set))
    {
      if(!get_message(active_fd.prev, buff))
      {
        close(active_fd.prev);
        active_fd.prev=0;
      }

      if(strstr(buff,"FND ")!=NULL)
      {
        Find_key(server,buff, active_fd);
      }
    }

    //Receiving a message from an Unknown node
    if(server.inRing && active_fd.temp && FD_ISSET(active_fd.temp, &read_set))
    {
      get_message(active_fd.temp, buff);
      //SUCCONF received
      if(strstr(buff,"SUCCCONF")!= NULL)
      {          //If there is only one-prev is 0
          if(!active_fd.prev)
          {
            active_fd.prev=active_fd.temp;
            active_fd.temp=0;
            create_msg(buff,server, "SUCC");
            send_message(active_fd.prev, buff);
          }
      }
      //NEW received
      else if(strstr(buff,"NEW ") !=NULL)
      {
        //If there more than one server
        if(active_fd.prev)
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

          active_fd.next = init_TCP_connect(server.Next_info.IP,server.Next_info.port);
          //Now the successor has to know his new predecessor
          send_message(active_fd.next,"SUCCCONF\n");
        }
      }
      //We are receiving the response to our key search request
      else if(strstr(buff,"KEY ")!=NULL)
      {
        Show_where_is_key(buff);
        close(active_fd.temp);
        active_fd.temp=0;
        create_EKEY(buff, udp_key);
        //printf("%s", buff);
        send_udp(active_fd.udp, buff, (struct sockaddr*)&addr, addrlen);
      }
      else
      {
        printf("unexpected message: abort\n");
        if(active_fd.prev == active_fd.temp)
          active_fd.prev = 0;
        close(active_fd.temp);
      }
    }
  }

  return 0;
}
