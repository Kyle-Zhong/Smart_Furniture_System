#ifndef _MENU_H_
#define _MENU_H_

#include "LCD.h"

void Menu_Creat();
int Menu_Judge(struct re_touch abs_value, int star_x, int star_y, int end_x, int end_y);
void Picture();
void Mp3_Player();
void Mp4_Player();
void Monitor();
void Serial_Init();
void Control();

#endif
