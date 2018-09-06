#ifndef _LCD_H_ 
#define _LCD_H_

void LCD_BMP_Init();
void LCD_Touch_Init();
void LCD_Point(int x, int y, int color);
void LCD_BMP(char *bmp_file, int star_x, int star_y);
void LCD_Clear();
struct re_touch Return_ABS();
int Touch_Area(int star_x, int star_y, int end_x, int end_y);
int Return_Dir();

//用来存放处理后的数据 
struct re_touch{
		int star_x;
		int star_y;
		int end_x;
		int end_y;
		int dir;
};

#endif
