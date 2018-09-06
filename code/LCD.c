#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <linux/input.h> 

#include "LCD.h"

int lcd_fd, bmp_fd, in_fd;//???tID 
int *p = NULL;//¡ä?¡¤?LCD¨®3¨¦??¨²¡ä?¦Ì??¡¤???? 

//LCD3?¨º??¡¥o¡¥¨ºy 
//D¨ª2?¡êo?T 
// ¡¤¦Ì???¦Ì¡êo?T 
//1|?¨¹¡êo
void LCD_BMP_Init(){
	//¡ä¨°?aLCD??¨º????t 
	lcd_fd = open("/dev/fb0", O_RDWR);
	
	if(lcd_fd == -1)
		perror("open the bmp color file error");		
	else
		printf("open the bmp color file successfuly!\n");
	
	//?¨²¡ä?¨®3¨¦? 
	p = mmap(NULL, 800 * 480 * 4, PROT_READ | PROT_WRITE, MAP_SHARED, lcd_fd, 0);
	
	if(p == MAP_FAILED)
		perror("mmap error");
	else
		printf("mmap successfuly!\n");
		
	//LCD_Clear();//3?¨º??¡¥???¨¢
}

//LCD3?¨º??¡¥o¡¥¨ºy 
//D¨ª2?¡êo?T 
// ¡¤¦Ì???¦Ì¡êo?T 
//1|?¨¹¡êo
void LCD_Touch_Init(){
	//¡ä¨°?aLCD¡ä£¤?t???t 
	in_fd = open("/dev/event0", O_RDWR);
	
	if(in_fd == -1)
		perror("open the bmp touch event file error");		
	else
		printf("open the bmp touch event file successfuly!\n");	
}

//LCD?¨¨¦Ì?o¡¥¨ºy
//D¨ª2?¡êox?¨¨¦Ì?¦Ì?X¡Á?¡À¨º¡ê?y?¨¨¦Ì?¦Ì?Y¡Á?¡À¨º¡ê?color?¨¨¦Ì???¨¦? 
//¡¤¦Ì???¦Ì¡êo?T 
//1|?¨¹¡êo
void LCD_Point(int x, int y, int color){
	*(p + y * 800 + x) = color;
} 

//LCD??¨º?BMP¨ª???o¡¥¨ºy
//D¨ª2?¡êobmp_file?a??¨º?¦Ì?¨ª?????¡Á?¡ê?star_xo¨ªstar_y?a??¨º?¨ª???¦Ì?¡Á¨®¨¦???¡Á?¡À¨º
//¡¤¦Ì???¦Ì¡êo?T 
//1|?¨¹¡êo
void LCD_BMP(char *bmp_file, int star_x, int star_y){	
	ssize_t r_ret;
	int i, j;
	int x, y; 
	
	//¡ä¨°?aBMP???t
	bmp_fd = open(bmp_file, O_RDWR);
		
	if(bmp_fd == -1)
		perror("open bmp error");
	else
		printf("open bmp file successfuly!\n");
		
	//?¨¢¨¨?BMP???t¨ºy?Y 
	lseek(bmp_fd, 0, SEEK_SET);
	
	char bmp_data_buf[54] = {0};//¨¦¨´?¡Â¡ä?¡¤?BMP¨ª??????t¨ºy?Y¨ºy¡Á¨¦
	
	r_ret = read(bmp_fd, bmp_data_buf, 54);
	
	if(r_ret == -1)
		perror("read the bmp file data error");
	else
		printf("read the bmp file data successfuly!\n");
		
	//??¨¨?¨ª????¨ªo¨ª?? 
	int bmp_x = 0, bmp_y = 0;//BMP¨ª????¨ªo¨ª??
		
	bmp_x = (bmp_data_buf[21] << 24) | (bmp_data_buf[20] << 16) | (bmp_data_buf[19] << 8) | (bmp_data_buf[18]);
	bmp_y = (bmp_data_buf[25] << 24) | (bmp_data_buf[24] << 16) | (bmp_data_buf[23] << 8) | (bmp_data_buf[22]);
	
	printf("\nthe wide of the picture:%d\n", bmp_x);
	printf("the high of the picture:%d\n\n", bmp_y);
	
	//?¨¬2a¨ª???¡ä¨®D?o¨ª??¨º?????¨º?¡¤?o?¡¤¡§
	if(bmp_x + star_x > 800 || bmp_y + star_y > 480){
		printf("the location of the picture is illegal!\n");
		
		return;
	} 
	
	char bmp_color_buf[bmp_x * bmp_y * 3];//¨¦¨´?¡Â¡ä?¡¤?BMP¨ª???¨¦?2¨º¨ºy?Y¨ºy¡Á¨¦ 
	int lcd_buf[bmp_x * bmp_y];//¨¦¨´?¡Â¡ä?¡¤?LCD¨ª???¨¦?2¨º¨ºy?Y¨ºy¡Á¨¦ 
	bmp_color_buf[bmp_x * bmp_y * 3] = 0;	
	lcd_buf[bmp_x * bmp_y] = 0;	
	
	//??¨¨?BMP¨¦?2¨º¨ºy?Y 
	lseek(bmp_fd, 54, SEEK_SET);//¨¬?1y???tD??¡é¨ºy?Y 
		
	r_ret = read(bmp_fd, bmp_color_buf, sizeof(bmp_color_buf));
	
	if(r_ret == -1)
		perror("read the bmp color data error");
	else
		printf("read the bmp color data successfuly!\n");
		
	//??BMP¨ºy?Y¡Áa???aLCD¨ºy?Y
	for(i = j = 0; i < bmp_x * bmp_y; i ++){
		lcd_buf[i] = 0X00 | (bmp_color_buf[j + 2] << 16) | (bmp_color_buf[j + 1] << 8) | (bmp_color_buf[j + 0] << 0);
		j += 3;
	}
	
	printf("translation the bmp color data successfuly!\n");
	
	//¡ã¡äLCD¨ºy?Y?¨²LCD¨¦??¨¨¦Ì?
	for(y = star_y; y < bmp_y + star_y; y ++)
		for(x = star_x; x < bmp_x + star_x; x ++)
			LCD_Point(x, y, lcd_buf[(bmp_y - (y - star_y)) * bmp_x + (x - star_x)]);
			
	close(bmp_fd);
}

//LCD???¨¢o¡¥¨ºy
//D¨ª2?¡êo?T
//¡¤¦Ì???¦Ì¡êo?T
//1|?¨¹¡êo
void LCD_Clear(){
	int x, y;
	
	for(y = 0; y < 480; y ++)
		for(x = 0; x < 800; x ++)
			LCD_Point(x, y, 0X00FFFFFF);		
} 

//?¨¬2a¡ä£¤?t?¨¢o¡¥¨ºy 
//D¨ª2?¡êo?T 
//¡¤¦Ì???¦Ì¡êo¨º???¨¤??a¨º¡Àx¡ê?y?¦Ì¡ä¡é¡ä??¨¢11¨¬?re_touch?D 
//1|?¨¹¡êo
struct re_touch Return_ABS(){
	struct input_event touch;
	struct re_touch re_touch;
 
	while(1){
		read(in_fd, &touch, sizeof(touch));
		
		if(touch.type == 0X03){
				if(touch.code == 0X00)
					re_touch.end_x = touch.value;
				else if(touch.code == 0X01)
					re_touch.end_y = touch.value;
		}
		
		if(touch.type == 0X01)
			if(touch.value == 0X00)
				break;
	}
	
	return re_touch;
}

int Touch_Area(int star_x, int star_y, int end_x, int end_y){
	struct re_touch touch_value;
	
	touch_value = Return_ABS();
	
	if(touch_value.end_x < end_x && touch_value.end_y < end_y && touch_value.end_x > star_x && touch_value.end_y > star_y)
		return 1;
	else
		return 0;
}

//?D?????¡¥¡¤??¨°o¡¥¨ºy
//D¨ª2?¡êo?T 
//¡¤¦Ì???¦Ì¡êo¡¤¦Ì??¨º??????¡¥¦Ì?¡¤??¨°?¦Ì¡ê?¨¦?¨®¨°??¡Á¨®¡¤?¡Àe?a¡êo1234¡ê?¨¨???¦Ì?¨°????¨°?a0 
//1|?¨¹¡êo
//     1
//    ?¨¹ 
//4?? 0 ?¨²2 
//   ?y 
//   3
//
//??¨º??¨¢¡Á?¡À¨º?¡§¨°? 
//  0?¨²x 
// ?y
// y 
int Return_Dir(){
	struct input_event touch;
	struct re_touch re_touch = {0, 0, 0, 0, 0};
	
	//?¨¬2a¡ã¡ä?? 
	while(1){
		read(in_fd, &touch, sizeof(touch));
		
		if(touch.type == 0X03){
			if(touch.code == 0X00)
				re_touch.star_x = touch.value;
			else if(touch.code == 0X01)
				re_touch.star_y = touch.value;
		}
		
		if(touch.type == 0X01)
			if(touch.value == 0X01)
				break;
	}
	
	//?¨¬2a?¨¦?a 
	while(1){
		read(in_fd, &touch, sizeof(touch));
		
		if(touch.type == 0X03){
				if(touch.code == 0X00)
					re_touch.end_x = touch.value;
				else if(touch.code == 0X01)
					re_touch.end_y = touch.value;
		}
		
		if(touch.type == 0X01)
			if(touch.value == 0X00)
				break;
	}
	
	int rela_x, rela_y;//x¡ê?y?¨¤????¨°?	
	
	rela_x = re_touch.end_x - re_touch.star_x;
	rela_y = re_touch.end_y - re_touch.star_y;
	
	//?D??¡¤??¨°
	if(re_touch.end_x == 0 || re_touch.end_y == 0) 
		return 0;
	else if(abs(rela_y) > abs(rela_x) && rela_y > 0)
		return 3;
	else if(abs(rela_y) > abs(rela_x) && rela_y < 0)
		return 1;
	else if(abs(rela_y) < abs(rela_x) && rela_x > 0)
		return 2;
	else if(abs(rela_y) < abs(rela_x) && rela_x < 0)
		return 4;
}

