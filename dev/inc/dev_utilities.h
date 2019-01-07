#ifndef __UTILITIES_H
#define __UTILITIES_H

#define ELEC "Electric: "
#define WATER "Water: "

#include "dev_lcd_i2c.h"

void dev_uti_show_electric_water(void);                         //Display Electric and Water on LCD
void dev_uti_show_month(void);
void dev_uti_make_confirmation(void);                           //Display confirm dailog on LCD
void dev_uti_string_copy(unsigned char *a, unsigned char *b);   //copy string b to string a
void dev_uti_blink(void);
void dev_uti_blink_init(void);
void dev_uti_reset_string(unsigned char* string, uint8_t position);
void dev_uti_prompt_to_insert(void);
void dev_uti_show_insertions(void);
void dev_uti_send_to_esp(void);

#endif
