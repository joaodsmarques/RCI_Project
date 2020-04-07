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
void entry_i();
//void sentry(all_info*);
void show(all_info);


#endif // INTERFACE_H_INCLUDED
