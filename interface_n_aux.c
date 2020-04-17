#include "interface_n_aux.h"
#include "structs_n_main.h"

//Prints a message
void print(const char* msg){
  printf("%s\n", msg);
}

//The main menu display
void Display_menu(){
  printf("|-------------------!Main Menu!------------------|\n");
  printf(" (Choose the number corresponding to your option)\n\n");
  printf("1.New i\n");
  printf("2.Entry i\n");
  printf("3.Sentry i\n");
  printf("4.Leave \n");
  printf("5.Show \n");
  printf("6.Find k i\n");
  printf("7.Exit \n");
}
//Used to get the option in the main Menu
//Returns the chosen option
//When the user selects a non menu option, returns -1
int get_option(){
  int option = 0;
  char buffer[VETOR_SIZE];
  if(!fgets(buffer, VETOR_SIZE, stdin))
    exit(1);
  sscanf(buffer,"%d", &option);
  if(option > 0 && option < 8)
    return option;
  else
    return -1;
}
//Returns the chosen key if it is valid
int new_i(){

  int key=0;
  char buffer[VETOR_SIZE];
  clrscreen();
  printf("Enter server key:\n");

  do{
    if(key <= 0 || key> RING_SIZE)
      printf("Key must be between the server limits(0-%d)\n",RING_SIZE);

    if(!fgets(buffer, VETOR_SIZE, stdin))
      exit(1);

  }while (sscanf(buffer,"%d", &key) != 1 ||  key <= 0 || key > RING_SIZE);
  //If the key is under the limits and valid
  return key;
}

//Receives the needed info to start the sentry command
//Saves it in the correct struct position
void sentry(all_info* sv_info){
  char buffer[VETOR_SIZE];
  int key =0;
  sv_info->key=new_i();
  //Key must be valid and under limits
  printf("Enter successor key:\n");
  do{
    if(key <= 0 || key> RING_SIZE)
      printf("Key must be between the server limits(0-%d)\n",RING_SIZE);

    if(!fgets(buffer, VETOR_SIZE, stdin))
      exit(1);

  }while (sscanf(buffer,"%d", &key) != 1 ||  key <= 0 || key > RING_SIZE);
  sv_info->succ_key=key;

  //Ip has dots and must be under the max lenght
  printf("Enter successor IP:\n");
  do{
    key=0;
    if(!fgets(buffer, IP_SIZE, stdin))
      exit(1);
    //IP always has dots
    if(strstr(buffer,".")==NULL)
      key=-1;

    //IP length has a limit
    if(strlen(buffer) >= IP_SIZE)
      key=-1;

    if(key==-1)
      printf("Invalid IP, try again\n");

  }while(key==-1);
  strcpy(sv_info->Next_info.IP,strtok(buffer, "\n"));

  //Port must be under the port limits
  printf("Enter successor Port:\n");
  do{
    key=0;
    if(!fgets(buffer, PORT_SIZE, stdin))
      exit(1);
    //Port limits
    if(sscanf(buffer,"%d", &key) < 1 ||  key <= 1024 || key > 64000)
      key=-1;

    if(key==-1)
      printf("Invalid port! It should be between 1024 and 64000\n");

  }while(key==-1);
  strcpy(sv_info->Next_info.port,strtok(buffer, "\n"));
}
//Similar to the prev function
void entry_i(all_info* sv_info){
  int key=0;
  char buffer[VETOR_SIZE];
  sv_info->key=new_i();

  printf("Enter IP:\n");
  do{
    key=0;
    if(!fgets(buffer, IP_SIZE, stdin))
      exit(1);
    //IP always has dots
    if(strstr(buffer,".")==NULL)
      key=-1;

    //IP length has a limit
    if(strlen(buffer) >= IP_SIZE)
      key=-1;

    if(key==-1)
      printf("Invalid IP, try again\n");

  }while(key==-1);
  strcpy(sv_info->Next_info.IP,strtok(buffer, "\n"));

  printf("Enter Port:\n");
  do{
    key=0;
    if(!fgets(buffer, PORT_SIZE, stdin))
      exit(1);
    //Port limits
    if(sscanf(buffer,"%d", &key) < 1 ||  key <= 1024 || key > 64000)
      key=-1;

    if(key==-1)
      printf("Invalid port! It should be between 1024 and 64000\n");

  }while(key==-1);
  strcpy(sv_info->Next_info.port,strtok(buffer, "\n"));
}

//Shows the server status:keys, IPs and ports connected to.
void show(all_info sv_info){
  printf("\n\n=========== SERVER STATUS ===========\n");
  //If our server is not initialized
  if(sv_info.inRing == false){
    printf("Status: DOWN\n");
    printf("Server IP: %s:%s\n", sv_info.Myinfo.IP, sv_info.Myinfo.port);
    printf("=====================================\n");
    printf("press enter to continue\n");
    return;
  }

  printf("\nStatus: OPERATIONAL\n");
  printf("Server IP: %s::%s\n", sv_info.Myinfo.IP, sv_info.Myinfo.port);
  printf("Server key: %d\n", sv_info.key);
  //If the ring has at least 2 nodes
  if(strcmp(sv_info.Next_info.port, sv_info.Myinfo.port))
    printf("Connected to %s::%s key: %d\n", sv_info.Next_info.IP, sv_info.Next_info.port, sv_info.succ_key);
  //The ring has at least 3 nodes
  if(strcmp(sv_info.SecondNext_info.port, sv_info.Myinfo.port) && strcmp(sv_info.SecondNext_info.port, sv_info.Next_info.port))
    printf("Second next server: %s::%s key: %d\n", sv_info.SecondNext_info.IP, sv_info.SecondNext_info.port, sv_info.second_succ_key);
  printf("===================================\n");
  printf("press enter to continue\n");
}

//Clear the terminal screen
void clrscreen()
{
  system("clear");
}
//Make a single string from 4 to 5 smaller strings.Used to create messages
void mystrcat(char* result,char* first,char* secnd,char* thrd,char* fourth,char* fifth){
  strcpy(result, first);
  strcat(result, " ");
  strcat(result, secnd);
  strcat(result, " ");
  strcat(result, thrd);
  strcat(result, " ");
  strcat(result, fourth);
  if (fifth != NULL)
  { //Just when there s a 5 element
    strcat(result, " ");
    strcat(result, fifth);
  }
  strcat(result,"\n");
}

//Depending on the type it creates different messages to send after
void create_msg(char* msg, all_info sv_info, const char* type)
{
  char key[12];
  memset(key,'\0',12);
  memset(msg,'\0',50);
  if (!strcmp(type, "SUCC"))
  {//2º successor info message
    sprintf(key,"%d",sv_info.succ_key);
    mystrcat(msg, "SUCC", key , sv_info.Next_info.IP, sv_info.Next_info.port, NULL);
  }
  else if(!strcmp(type, "NEW"))
  {//New ring node entering message
    sprintf(key,"%d",sv_info.key);
    mystrcat(msg,"NEW", key, sv_info.Myinfo.IP, sv_info.Myinfo.port, NULL);
  }
  else if(!strcmp(type, "KEY"))
  { //Message telling the key is in the successor.Second_key was changed temporarly to the key we are finding
    sprintf(msg,"KEY %d %d %s %s\n",sv_info.second_succ_key, sv_info.succ_key,
    sv_info.Next_info.IP,sv_info.Next_info.port);
  }
  else if(!strcmp(type, "EFND"))
  {//UDP entrance message, finding the key we need
    sprintf(msg,"EFND %d", sv_info.key);
  }
}

//Saves the data from a new connection in the specific area
void parse_new(char* msg, server_info* server, int* key){
  char *aux;
  aux = (char*) malloc(sizeof(char) * 50);
  memset(aux,'\0',50);
  strcpy(aux,msg);
  strtok(aux," ");//Reads the flag
  *key = atoi(strtok(NULL, " "));//Reads the keys
  strcpy(server->IP, strtok(NULL," "));//Reads the IP
  strcpy(server->port, strtok(NULL,"\n"));//Read the port
  free(aux);

}

//Same as the prev function but used for the udp entrance
int parse_EKEY(char*msg, all_info *server){
  char *aux;
  int find_k=0;

  aux = (char*) malloc(sizeof(char) * 50);
  memset(aux,'\0',50);
  strcpy(aux,msg);
  strtok(aux," ");
  find_k=atoi(strtok(NULL," "));
  server->succ_key = atoi(strtok(NULL, " "));

  //Key is already in the ring
  if(server->succ_key == find_k)
  { //Reset the server info
    server->succ_key=-1;
    free(aux);
    return -1;
  }
  else
  {//Have all we need to enter the server
    strcpy(server->Next_info.IP, strtok(NULL," "));
    strcpy(server->Next_info.port, strtok(NULL,"\0"));
    free(aux);
    return 0;
  }
}
//Calculates the distance of the key to me and my successor
//Return values:
//0-If the successor does not have the Key_
//1-The sucessor has the key
int Key_Distance(int find_key, int my_key,int succ_key)
{
  int diff_succ=0;
  int my_diff=0;

  //eg Ringsize 16: d(9,3)=10
  //see the distance counter-clockwise from the searching key to the key we are testing
  if(find_key>my_key)
    my_diff=(RING_SIZE+my_key)-find_key;

  else
    my_diff=my_key-find_key;

  if(find_key>succ_key)
    diff_succ=(RING_SIZE+succ_key)-find_key;

  else
    diff_succ=succ_key-find_key;

  //The key is not in my successor
  if(diff_succ>my_diff)
    return 0;
  //My successor has the key
  else
    return 1;

}
//Receives the find_key message and displays the result
void Show_where_is_key(char* message)
{
  char*IP;
  char*PORT;
  int find_key=0;
  int node_key=0;
  char aux[50];
  memset(aux,'\0',50);
  strcpy(aux,message);
  strtok(aux," ");
  find_key=atoi(strtok(NULL," "));
  node_key=atoi(strtok(NULL," "));
  IP = strtok(NULL," ");
  PORT = strtok(NULL,"\n");
  clrscreen();
  printf("\n=============================================\n");
  printf("I FOUND WHERE THE KEY %d IS KEPT\n",find_key);
  printf("=============================================\n");
  printf("Server who keeps it:\n");
  printf("IP & PORT: %s::%s\n",IP,PORT);
  printf("Key:%d\n", node_key);
  printf("=============================================\n");
  printf("(Press Enter to return to the main menu)\n");
}
//Create the EKEY message with the defined format
void create_EKEY(char * msg, int key){
  char *aux;
  aux = (char*) malloc(sizeof(char) * 50);
  memset(aux,'\0',50);
  strtok(msg," ");
  strtok(NULL," ");
  sprintf(aux,"EKEY %d ",key);
  strcat(aux, strtok(NULL," "));
  strcat(aux," ");
  strcat(aux, strtok(NULL," "));
  strcat(aux," ");
  strcat(aux, strtok(NULL,"\n"));
  memset(msg, '\0', 50);
  strcpy(msg, aux);

  free(aux);

}

//Before starting the key search in the hash table
//Verifies if the chosen key is valid and creates the sending message for the operation
void Start_Search(char* msg,all_info _server)
{
  memset(msg,'\0',50);

  int key=0;

  char buffer[VETOR_SIZE];
  printf("Enter server key:\n");

  do{
    if(key <= 0 || key> RING_SIZE)
      printf("Key must be between the server limits(0-%d)\n",RING_SIZE);

    if(!fgets(buffer, VETOR_SIZE, stdin))
      exit(1);


  }while (sscanf(buffer,"%d", &key) != 1 ||  key <= 0 || key > RING_SIZE);
  //Creating the message to be sent or analyzed
  sprintf(msg,"FND %d %d %s %s\n",key, _server.key,_server.Myinfo.IP, _server.Myinfo.port);

}
