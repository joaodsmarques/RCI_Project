#include "interface.h"
#include "Structs_n_main.h"
#include <stdio.h>
#include <stdbool.h>

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
  do{
    fgets(buffer, VETOR_SIZE, stdin);
  }while (sscanf(buffer,"%d", &option) != 1 || option < 1 || option > 7);
  return option;
}

int new_i(){

  int key=0;
  char buffer[VETOR_SIZE];
  printf("Enter server key:\n");

  do{
    fgets(buffer, VETOR_SIZE, stdin);
    if(key < 0)
      printf("key must be greater than 0\n");
  }while (sscanf(buffer,"%d", &key) != 1 ||  key < 0);
  printf("%d\n", key);
  return key;
}

