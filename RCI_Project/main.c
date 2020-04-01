/*
Programa elaborado para a disciplina XXXX

Criadores:
Guilherme Guerreiro
João Marques

Explicação
Obrigado!
*/
#include "Structs_n_main.h"
#include "interface_n_aux.h"
#include "Network.h"


int main(int argc, char * argv[])
{
  int menu = 0;
  all_info * server;

  server = MemoryAlloc();
  server = startup(argc, argv, server);

  system("clear");
  while(menu != -1)
  {
    //Mostra as opções que estao no menu e recolhe a escolhida na
    //variavel de entrada
    //system("clear");
    Display_main_menu();
    menu = get_option();

    switch (menu)
    {
        case 1:
            if(server->inRing == false)
            {
              server=Choose_key(server);
              server->inRing = true;
              server = NewServer_Heart(server);
            }
            else
            {
              printf("Server already in ring\nChoose another option\n");
            }
            //TCP_Server_Connect(MyCloset);

        break;

        case 2:

        break;

        case 3:
          server = StartSucci(server);
          server = SentryServer_Heart(server);
        break;

        case 4:
          //Faz cenas
        break;

        case 5:

        break;

        case 6:

        break;

        case 7:
          menu = -1;
        break;
    }


  }

  printf("It was a pleasure, son\n");

  free(server);

  return 0;
}
