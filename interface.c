#include "interface.h"
#include "structs_n_main.h"
#include <stdio.h>
#include <stdbool.h>
#include <strings.h>

void print(const char* msg){
  printf("%s\n", msg);
}

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

int new_i(){

  int key=0;
  char buffer[VETOR_SIZE];
  printf("Enter server key:\n");

  do{
    if(!fgets(buffer, VETOR_SIZE, stdin))
      exit(1);
    if(key < 0)
      printf("key must be greater than 0\n");
  }while (sscanf(buffer,"%d", &key) != 1 ||  key < 0);
  return key;
}

void sentry(all_info* sv_info){
  char buffer[VETOR_SIZE];
  sv_info->key=new_i();

  printf("Enter successor key:\n");
  if(!fgets(buffer, VETOR_SIZE, stdin))
    exit(0);
  sscanf(buffer,"%d", &(sv_info->succ_key));
  printf("Enter successor IP:\n");
  if(!fgets(buffer, IP_SIZE, stdin))
    exit(0);
  strcpy(sv_info->Next_info.IP,strtok(buffer, "\n"));

  printf("Enter successor Port:\n");
  if(!fgets(buffer, PORT_SIZE, stdin))
    exit(0);
  strcpy(sv_info->Next_info.port,strtok(buffer, "\n"));
}

void entry_i(all_info* sv_info){
  char buffer[VETOR_SIZE];
  sv_info->key=new_i();

  printf("Enter your key:\n");
  if(!fgets(buffer, VETOR_SIZE, stdin))
    exit(0);
  sscanf(buffer,"%d", &(sv_info->key));
  printf("Enter recving IP:\n");
  if(!fgets(sv_info->Next_info.IP, IP_SIZE, stdin))
    exit(0);
  printf("Enter recving Port:\n");
  if(!fgets(sv_info->Next_info.port, PORT_SIZE, stdin))
    exit(0);
}

void show(all_info sv_info){
  printf("\n\n=========== SERVER STATUS ===========\n");
  if(sv_info.inRing == false){
    printf("Status: DOWN\n");
    printf("Server IP: %s:%s\n", sv_info.Myinfo.IP, sv_info.Myinfo.port);
    printf("=====================================\n");
    printf("press enter to continue\n");
    return;
  }
  printf("Status: OPERATIONAL\n");
  printf("Server IP: %s::%s\n", sv_info.Myinfo.IP, sv_info.Myinfo.port);
  printf("Server key: %d\n", sv_info.key);
  if(strcmp(sv_info.Next_info.port, sv_info.Myinfo.port))
    printf("Connected to %s::%s key: %d\n", sv_info.Next_info.IP, sv_info.Next_info.port, sv_info.succ_key);
  if(strcmp(sv_info.SecondNext_info.port, sv_info.Myinfo.port))
    printf("Second next server: %s::%s key: %d\n", sv_info.SecondNext_info.IP, sv_info.SecondNext_info.port, sv_info.second_succ_key);
  printf("===================================\n");
  printf("press enter to continue\n");
}

void clrscreen(){
  printf("\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n");
}

void mystrcat(char* result,char* first,char* secnd,char* thrd,char* fourth,char* fifth){
  strcpy(result, first);
  strcat(result, " ");
  strcat(result, secnd);
  strcat(result, " ");
  strcat(result, thrd);
  strcat(result, " ");
  strcat(result, fourth);
  if (fifth != NULL)
  {
    strcat(result, " ");
    strcat(result, fifth);
  }
  strcat(result,"\n");
}

void create_msg(char* msg, all_info sv_info, const char* type)
{
  char key[12];
  memset(key,'\0',12);
  memset(msg,'\0',50);
  if (!strcmp(type, "SUCC"))
  {
    sprintf(key,"%d",sv_info.succ_key);
    mystrcat(msg, "SUCC", key , sv_info.Next_info.IP, sv_info.Next_info.port, NULL);
  }
  else if(!strcmp(type, "NEW"))
  {
    sprintf(key,"%d",sv_info.key);
    mystrcat(msg,"NEW", key, sv_info.Myinfo.IP, sv_info.Myinfo.port, NULL);
  }
}

//Saves the data in the specific area
void parse_new(char* msg, server_info* server, int* key){
  char *aux;
  aux = (char*) malloc(sizeof(char) * 50);
  memset(aux,'\0',50);
  strcpy(aux,msg);
  strtok(aux," ");
  *key = atoi(strtok(NULL, " "));
  strcpy(server->IP, strtok(NULL," "));
  strcpy(server->port, strtok(NULL,"\n"));
  free(aux);
  return;
}
