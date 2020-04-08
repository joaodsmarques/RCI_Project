#ifndef INTERFACE_H_INCLUDED
#define INTERFACE_H_INCLUDED
//#include "TCP.h"
#include "structs_n_main.h"
#include <stdlib.h>


void clrscreen();
void Display_menu();
void print(const char*);
int get_option();
int new_i();
void sentry(all_info*);
void entry_i();
void show(all_info);
int message_analysis(char*, const char*);
void mystrcat(char*,char*,char*,char*,char*,char*);
void create_msg(char* msg, all_info sv_info, const char* type);
void parse_new(char*, server_info*, int*);

#endif // INTERFACE_H_INCLUDED
