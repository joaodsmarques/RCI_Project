#ifndef INTERFACE_N_AUX_H_INCLUDED
#define INTERFACE_N_AUX_H_INCLUDED
//#include "TCP.h"
#include "structs_n_main.h"

//Main menu display functions
void clrscreen();
void Display_menu();
void show(all_info);
void print(const char*);

//Gets the main menu option
int get_option();
//Getting options and info from stdin and saving them in the right spot
int new_i();
void sentry(all_info*);
void entry_i();

//Creating Messages
void mystrcat(char*,char*,char*,char*,char*,char*);
void create_msg(char* msg, all_info sv_info, const char* type);
void parse_new(char*, server_info*, int*);
int parse_EKEY(char*, all_info *);
void create_EKEY(char *, int);

//FIND KEY interface and auxiliar funcions
void Show_where_is_key(char*);
int Key_Distance(int , int ,int );
void Start_Search(char* ,all_info);

#endif // INTERFACE_N_AUX_H_INCLUDED
