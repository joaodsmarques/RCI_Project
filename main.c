/*
This program was made as an evaluation task for RCI.

It makes possible the creation of a ring server starting with a single node
where the other are added by the "Entry" and "Sentry" commands.
After being created, the ring is assumed as an Hash table where its keys are
distributed following the concept: "The most closer node after the key is saving it".
The client can find where all the keys are being saved.

It was a really challenging work where we had to recode over and over again
because we never had contact with "Socket programming before RCI, but after all
here we are!

All we can say now is: Enjoy your connections!

Authors:
Guilherme Guerreiro 87010
João Marques 90114

IST students

Thank you for reading!
*/

#include "structs_n_main.h"
#include "interface_n_aux.h"
#include "network.h"



//Program execution
int main(int argc, char* argv[])
{
  clrscreen();
  int maxfd = 0;
  int count = 0;//Resend the udp message flag
  int udp_key;
  int pending=0;//waiting for the key finder after udp asking
  char buff[MSG_SIZE];//message receiving
  struct addrinfo* udp_addr;
  struct sockaddr_in addr;
  socklen_t addrlen;
  struct timeval *timeout=NULL;//Time of the timeout
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

    //The program heart! Deals with all the traffic and connextions
  	if(!select(maxfd+1, &read_set, (fd_set*) NULL, (fd_set*) NULL, timeout))
    {
      //If no fd is set, there was a timeout sending the udp message
      if(!server.inRing)
      { //If count is 0, lets resend the udp message
        if(!(count++))
        {
          create_msg(buff, server, "EFND");
          send_udp(active_fd.udp, buff, udp_addr->ai_addr, udp_addr->ai_addrlen);
          udp_timeout.tv_sec = 5;
          udp_timeout.tv_usec = 0;
          timeout = &udp_timeout;
        }
        else
        {//After sendig 2 times the message, if we are not set yet, the connection failed
          printf("connection timed out\n");
          printf("Ring Entrance Failed\n");
          close(active_fd.udp);
          active_fd.udp=0;
          count=0;
          timeout=NULL;
          printf("press enter to continue\n");
        }
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
            clrscreen();
      		}
          else
          {
            clrscreen();
            printf("You must leave the current ring first\n");
          }
          Display_menu();
    		break;
    		case 2:  //ENTRY i.Choose a key and enter in the ring if the key is available
          if(!server.inRing)
          {
            entry_i(&server);
            active_fd.udp = init_UDPcl(&server, &udp_addr);
            create_msg(buff, server, "EFND");
            send_udp(active_fd.udp, buff, udp_addr->ai_addr, udp_addr->ai_addrlen);
            udp_timeout.tv_sec = 5;
            udp_timeout.tv_usec = 0;
            timeout = &udp_timeout;
          }
          else
          {
            clrscreen();
            printf("You must leave the current ring first\n");
            Display_menu();
          }
    		break;
    		case 3://Sentry command. You know where you can enter and how. just puts the info to do it
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
    		case 4://LEAVE. You can leave the ring any time
          if(server.inRing)
          {
            printf("You left the ring\n");
            close_all(&active_fd, &server);
          }
          else
          {
            clrscreen();
            printf("You must enter in a Ring First!!\n");
            Display_menu();
          }
    		break;
    		case 5: //SHOW the actual server state.
          clrscreen();
          show(server);
    		break;
    		case 6: //Find where a certain key is kept in the ring
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
    		case 7://Leave the program
          if(server.inRing)
            printf("You must leave the Ring First!!\n");
          else
          {
            close_all(&active_fd, &server);
            printf("It was a pleasure connecting with you!!!\n\n");
            exit(0);
          }
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
		{ //If we are not in the ring, we are using the entry command
      if(!server.inRing)
      {
        memset(buff,'\0',50);
        recv_udp(active_fd.udp, buff, NULL, NULL);
        timeout = NULL;
        close(active_fd.udp);
        active_fd.udp=0;

        if(parse_EKEY(buff, &server)==-1)//Key is taken
        {
          clrscreen();
          printf("--Key already taken, try again!--\n");
          printf("(Press Enter to go to Main Menu)\n");
        }
        else
        {//Everything set to the connection
          active_fd.udp = init_UDPsv(&server);
          active_fd.listen = init_TCP_Listen(&server);
          active_fd.next = init_TCP_connect(server.Next_info.IP,server.Next_info.port); //connects to successor
          //Sends the first message to the successor
          create_msg(buff, server, "NEW");
          send_message(active_fd.next, buff);
          server.inRing = true;
        }
      }
      else//Receiving and udp task from an entering server
      {
        memset(buff,'\0',50);
        recv_udp(active_fd.udp, buff, (struct sockaddr*)&addr, &addrlen);
        strtok(buff," ");
        sscanf(strtok(NULL,"\0"),"%d",&udp_key);//Discovers the key we are going to find
        memset(buff,'\0', 50);
        //writes the sending message
        sprintf(buff,"FND %d %d %s %s\n",udp_key, server.key, server.Myinfo.IP, server.Myinfo.port);
        switch(Find_key(server, buff, active_fd))
        {
          case 0://If I'm not alone and the key isnt in my succ
            pending=1;//We have to wait for the key to be searched
          break;
          case 1://If I'm alone in the ring
            memset(buff,'\0', 50);
            sprintf(buff,"EKEY %d %d %s %s",udp_key, server.key, server.Myinfo.IP, server.Myinfo.port);
            send_udp(active_fd.udp, buff, (struct sockaddr*)&addr, addrlen);
          break;
          case 2://The successor has the key and I'm the client
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


    //If connection is set and there is something to read
    if(server.inRing && active_fd.next && FD_ISSET(active_fd.next, &read_set))
    {
      //If read returns 0, the connections was lost
      if(!get_message(active_fd.next,buff))
      {
        close(active_fd.next);

        printf("My successor left\n");
        //CLEAN SECOND SUCC
        strcpy(server.Next_info.port, server.SecondNext_info.port);
        server.succ_key=server.second_succ_key;

        //if there is only 1 node left, we dont need to make a new connection
        if(strstr(server.Next_info.port,server.Myinfo.port)!=NULL && strstr(server.Next_info.IP,server.Myinfo.IP)!=NULL)
        {
          active_fd.next=0;
        }
        else//Needs to connect to the 2ºsuccessor which is now the successor
        {
          active_fd.next = init_TCP_connect(server.Next_info.IP,server.Next_info.port);
          create_msg(buff, server, "SUCC");//Gives the successor to the predecessor
          send_message(active_fd.prev,buff);
          send_message(active_fd.next,"SUCCCONF\n");//Informs the successor about his existance
        }

      }
      //Let's receive the Second successor
      if(strstr(buff,"SUCC ")!=NULL)
      {
        parse_new(buff, &(server.SecondNext_info), &(server.second_succ_key));
      }
      else if(strstr(buff,"NEW ")!= NULL)//We are getting a new successor
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
        send_message(active_fd.next,"SUCCCONF\n");//Informs the new successor about its new predecessor
      }
    }
    //Predecessor is talking
    if(server.inRing && active_fd.prev && FD_ISSET(active_fd.prev, &read_set))
    {
      if(!get_message(active_fd.prev, buff))//Predecessor left
      {
        printf("My predecessor left\n");
        close(active_fd.prev);
        active_fd.prev=0;
      }

      if(strstr(buff,"FND ")!=NULL)//We are searching for a key
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
      {
          if(!active_fd.prev)//Predecessor fd is always 0 when waiting for the new predecessor
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
        //If there are more than one server
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
      { //Just wants to know where is the key
        Show_where_is_key(buff);
        close(active_fd.temp);
        active_fd.temp=0;
        if(pending==1) //If we were waiting this to inform the entry server
        {
          create_EKEY(buff, udp_key);
          send_udp(active_fd.udp, buff, (struct sockaddr*)&addr, addrlen);
          pending=0;
        }
      }
      else//Receiving other messages for mistake
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
