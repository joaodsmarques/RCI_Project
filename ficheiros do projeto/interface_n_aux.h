#ifndef INTERFACE_H_INCLUDED
#define INTERFACE_H_INCLUDED

#include "Structs_n_main.h"

all_info* startup(int , char*[], all_info *);

void Display_main_menu();
void Display_new_menu();

int get_option();
int get_option_inserver();

all_info* MemoryAlloc();

all_info* Choose_key(all_info*);


#endif // INTERFACE_H_INCLUDED
