#include "interface_n_aux.h"


all_info* startup(int _argc, char* _argv[], all_info * _server)
{

  if (_argc != 3)
    exit(0);

  else if(atoi(_argv[2]) <= 1023 || atoi(_argv[2]) > 64000)
  {
    printf("The port must be a number between 1024 and 64000\n");
    exit(0);
  }

  else
  {
    strcpy(_server->Myinfo.port, _argv[2]);
    strcpy(_server->Myinfo.IP, _argv[1]);
    _server->key=-1;
    _server->succ_key=-1;
    _server->inRing = false;

  }

  return _server;
}

void Display_main_menu()
{
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

void Display_new_menu()
{
  printf("|-------------------!InRing Menu!------------------|\n");
  printf("   (Choose the number corresponding to your option)\n\n");
  printf("1.Leave \n");
  printf("2.Show \n");
  printf("3.Find k i\n");
}

int get_option()
{

  int option = 0;
  char buffer[VETOR_SIZE];

  do
  {
    fgets(buffer, VETOR_SIZE, stdin);
  }while (sscanf(buffer,"%d", &option) != 1 || option < 1 || option > 7);

  return option;
}

int get_option_inserver()
{

  int option = 0;
  char buffer[VETOR_SIZE];

  do
  {
    fgets(buffer, VETOR_SIZE, stdin);
  }while (sscanf(buffer,"%d", &option) != 1 || option < 1 || option > 3);

  return option;
}

all_info* MemoryAlloc()
{
    all_info* aux=NULL;

    aux=(all_info*)malloc(sizeof(all_info));	//alocacao de memoria

    if(aux==NULL)	//Caso a alocacao seja defeituosa
    {
        printf("Error: Couldn't Allocate Closet Memory\n");
        exit(0);
    }

    return aux;	//Retorna o closet deste elemento em especifico
}


all_info * Choose_key(all_info* ptr)
{
  int aux=-1;
  char buffer[VETOR_SIZE];

  do
  {
    system("clear");

    printf("|-------------------!New i!------------------|\n");
    printf(" (Enter the number you want for your server between 0 and 15)\n\n");

    fgets(buffer, VETOR_SIZE, stdin);

  }while (sscanf(buffer,"%d", &aux) != 1 || aux < 0 || aux > 15);

  ptr->key = aux;
  ptr->succ_key = aux;

  //Server is formed by just 1 memer.It's an all in 1.
  strcpy(ptr->Next_info.port, ptr->Myinfo.port);
  strcpy(ptr->Prev_info.port, ptr->Myinfo.port);
  strcpy(ptr->SecondNext_info.port, ptr->Myinfo.port);
  strcpy(ptr->Next_info.IP, ptr->Myinfo.IP);
  strcpy(ptr->Prev_info.IP, ptr->Myinfo.IP);
  strcpy(ptr->SecondNext_info.IP, ptr->Myinfo.IP);


  return ptr;
}
/*
Função que analisa mensagens do tipo NEW.
Entrada tem o buffer da mensagem, a struct do servidor e um inteiro que indica
onde guardamos os dados na estrutura all_info
-1 - em todo o lado(servidor era de no UNICO)
0-no predecessor
1-no sucessor
2-no duplo sucessor
*/
all_info* Message_Analysis (char _buffer[VETOR_SIZE], all_info* myserver, int where)
{
  //NEW 8 8.127.0.0.1 8.3300\n

  char* token = NULL;
  const char dot[2]={"."};
  const char space[2]={" "};
  const char fin[2]={"\n"};
  char supbuffer[VETOR_SIZE];

  //supbuffer: in case we need to resend the buffer message to another
  strcpy(supbuffer,_buffer);
  if(strstr(_buffer,"NEW")!=NULL)
  {
    token = strtok(supbuffer,fin);

    if(token==NULL)
    {
      printf("Erro a extrair string\n");
      exit(EXIT_FAILURE);
    }
    strcpy(supbuffer, token);
  }
  //Verifica se tem a palavra NEW na mensagem
  if(strstr(_buffer,"NEW")!=NULL || strstr(_buffer,"SUCC")!=NULL)
  {
    //Reads the first word
    token = strtok( _buffer,space);

    if(token==NULL)
    {
      printf("Erro a extrair string\n");
      exit(EXIT_FAILURE);
    }
    //Reads the key
    token=strtok(NULL, space);
    if(where==1 || where ==-1)
      myserver->succ_key = atoi(token);

    //Reads until the beginning of IP
    token=strtok(NULL, dot);

    //Reads and copies the IP
    token=strtok(NULL, space);
    if(where==0 || where==-1)
      strcpy(myserver->Prev_info.IP,token);

    if(where== -1 || where == 1)
      strcpy(myserver->Next_info.IP,token);

    if(where==2)
      strcpy(myserver->SecondNext_info.IP,token);

    //Reads until the port beginning
    token=strtok(NULL, dot);

    //Reads and copies the port
    token=strtok(NULL, fin);
    if(where==-1 || where==0)
      strcpy(myserver->Prev_info.port,token);

    if(where==-1 || where==1)
      strcpy(myserver->Next_info.port,token);

    if(where==2)
      strcpy(myserver->SecondNext_info.port,token);

    strcpy(_buffer, supbuffer);

    //In the first insertion the second succ is himself
    if(where==-1)
    {
      strcpy(myserver->SecondNext_info.IP,myserver->Myinfo.IP);
      strcpy(myserver->SecondNext_info.port,myserver->Myinfo.port);
    }
  }

  return myserver;
}


void ServerState(all_info* myserver)
{

  system("clear");
  printf("|-----Server State-----|\n\n");
  printf("My key:%d\n", myserver->key);
  printf("Succ key:%d\n", myserver->succ_key);
  printf("My IP & PORT:%s:%s\n", myserver->Myinfo.IP,myserver->Myinfo.port);
  printf("Succ IP & PORT:%s:%s\n", myserver->Next_info.IP,myserver->Next_info.port);
  printf("PRED IP & PORT:%s:%s\n", myserver->Prev_info.IP,myserver->Prev_info.port);
  printf("2º Succ IP & PORT:%s:%s\n", myserver->SecondNext_info.IP,myserver->SecondNext_info.port);
  printf("(Press enter to exit Server Status)\n");

}


//Recolha de dados do comado de entradqa. A
//Verificar analise e possiveis erros de escrita
all_info* StartSucci(all_info* myserver)
{
  char _buffer[VETOR_SIZE];
  char * token=NULL;
  const char dot[2]={"."};
  const char space[2]={" "};
  const char fin[2]={"\n"};
  int errorflag=-1;
  //sentry i boot boot.IP boot.TCP

  system("clear");

  printf("|-------------------!Sentry i!------------------|\n");
  printf("         Write down the Sentry Command\n");
  printf("       (sentry i boot boot.IP boot.TCP)\n\n");


  while ( errorflag == -1)
  {
    fgets(_buffer, VETOR_SIZE, stdin);

    //If there are no spaces, the code is incorret
    if(strstr(_buffer," ")!=NULL)
    {
      //Reads the first word
      token = strtok( _buffer,space);

      if(token==NULL)
      {
        printf("String Extration was unsuccessful\n");
        exit(EXIT_FAILURE);
      }

      //if the command is corret
      if(strstr(token,"sentry")!=NULL)
      {
        //The key
        token=strtok(NULL, space);
        myserver->key = atoi(token);

        //The succ key
        token=strtok(NULL, space);
        myserver->succ_key = atoi(token);

        //Between the dot and the space we have the IP
        token=strtok(NULL, dot);
        token=strtok(NULL, space);
        strcpy(myserver->Next_info.IP,token);

        //Between the dot and the \n we have the port
        token=strtok(NULL, dot);
        token=strtok(NULL, fin);
        strcpy(myserver->Next_info.port,token);

        errorflag=0;
      }
    }
    if(errorflag!=0)
    {
      system("clear");

      printf("|-------------------!Sentry i!------------------|\n");
      printf("    You put a wrong command, let's try again!\n");
      printf("         Write down the Sentry Command\n");
      printf("       (sentry i boot boot.IP boot.TCP)\n\n");
    }
  }

  /*printf("Vamos ver se esta tudo ok\n");
  printf("My key:%d\n", myserver->key);
  printf("Succ key:%d\n", myserver->succ_key);
  printf("My IP & PORT:%s:%s\n", myserver->Myinfo.IP,myserver->Myinfo.port);
  printf("Succ IP & PORT:%s:%s\n", myserver->Next_info.IP,myserver->Next_info.port);*/


  return myserver;

}
