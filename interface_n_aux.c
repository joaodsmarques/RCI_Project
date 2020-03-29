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
  printf("|-------------------!New Server Menu!------------------|\n");
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

  return ptr;
}
