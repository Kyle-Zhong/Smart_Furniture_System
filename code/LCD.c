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
int *p = NULL;//��?��?LCD��3��??����?��??��???? 

//LCD3?��??��o����y 
//D��2?��o?T 
// ����???�̡�o?T 
//1|?����o
void LCD_BMP_Init(){
	//�䨰?aLCD??��????t 
	lcd_fd = open("/dev/fb0", O_RDWR);
	
	if(lcd_fd == -1)
		perror("open the bmp color file error");		
	else
		printf("open the bmp color file successfuly!\n");
	
	//?����?��3��? 
	p = mmap(NULL, 800 * 480 * 4, PROT_READ | PROT_WRITE, MAP_SHARED, lcd_fd, 0);
	
	if(p == MAP_FAILED)
		perror("mmap error");
	else
		printf("mmap successfuly!\n");
		
	//LCD_Clear();//3?��??��???��
}

//LCD3?��??��o����y 
//D��2?��o?T 
// ����???�̡�o?T 
//1|?����o
void LCD_Touch_Init(){
	//�䨰?aLCD�䣤?t???t 
	in_fd = open("/dev/event0", O_RDWR);
	
	if(in_fd == -1)
		perror("open the bmp touch event file error");		
	else
		printf("open the bmp touch event file successfuly!\n");	
}

//LCD?����?o����y
//D��2?��ox?����?��?X��?������?y?����?��?Y��?������?color?����???��? 
//����???�̡�o?T 
//1|?����o
void LCD_Point(int x, int y, int color){
	*(p + y * 800 + x) = color;
} 

//LCD??��?BMP��???o����y
//D��2?��obmp_file?a??��?��?��?????��?��?star_xo��star_y?a??��?��???��?������???��?����
//����???�̡�o?T 
//1|?����o
void LCD_BMP(char *bmp_file, int star_x, int star_y){	
	ssize_t r_ret;
	int i, j;
	int x, y; 
	
	//�䨰?aBMP???t
	bmp_fd = open(bmp_file, O_RDWR);
		
	if(bmp_fd == -1)
		perror("open bmp error");
	else
		printf("open bmp file successfuly!\n");
		
	//?����?BMP???t��y?Y 
	lseek(bmp_fd, 0, SEEK_SET);
	
	char bmp_data_buf[54] = {0};//����?�¡�?��?BMP��??????t��y?Y��y����
	
	r_ret = read(bmp_fd, bmp_data_buf, 54);
	
	if(r_ret == -1)
		perror("read the bmp file data error");
	else
		printf("read the bmp file data successfuly!\n");
		
	//??��?��????��o��?? 
	int bmp_x = 0, bmp_y = 0;//BMP��????��o��??
		
	bmp_x = (bmp_data_buf[21] << 24) | (bmp_data_buf[20] << 16) | (bmp_data_buf[19] << 8) | (bmp_data_buf[18]);
	bmp_y = (bmp_data_buf[25] << 24) | (bmp_data_buf[24] << 16) | (bmp_data_buf[23] << 8) | (bmp_data_buf[22]);
	
	printf("\nthe wide of the picture:%d\n", bmp_x);
	printf("the high of the picture:%d\n\n", bmp_y);
	
	//?��2a��???�䨮D?o��??��?????��?��?o?����
	if(bmp_x + star_x > 800 || bmp_y + star_y > 480){
		printf("the location of the picture is illegal!\n");
		
		return;
	} 
	
	char bmp_color_buf[bmp_x * bmp_y * 3];//����?�¡�?��?BMP��???��?2����y?Y��y���� 
	int lcd_buf[bmp_x * bmp_y];//����?�¡�?��?LCD��???��?2����y?Y��y���� 
	bmp_color_buf[bmp_x * bmp_y * 3] = 0;	
	lcd_buf[bmp_x * bmp_y] = 0;	
	
	//??��?BMP��?2����y?Y 
	lseek(bmp_fd, 54, SEEK_SET);//��?1y???tD??�騺y?Y 
		
	r_ret = read(bmp_fd, bmp_color_buf, sizeof(bmp_color_buf));
	
	if(r_ret == -1)
		perror("read the bmp color data error");
	else
		printf("read the bmp color data successfuly!\n");
		
	//??BMP��y?Y��a???aLCD��y?Y
	for(i = j = 0; i < bmp_x * bmp_y; i ++){
		lcd_buf[i] = 0X00 | (bmp_color_buf[j + 2] << 16) | (bmp_color_buf[j + 1] << 8) | (bmp_color_buf[j + 0] << 0);
		j += 3;
	}
	
	printf("translation the bmp color data successfuly!\n");
	
	//���LCD��y?Y?��LCD��??����?
	for(y = star_y; y < bmp_y + star_y; y ++)
		for(x = star_x; x < bmp_x + star_x; x ++)
			LCD_Point(x, y, lcd_buf[(bmp_y - (y - star_y)) * bmp_x + (x - star_x)]);
			
	close(bmp_fd);
}

//LCD???��o����y
//D��2?��o?T
//����???�̡�o?T
//1|?����o
void LCD_Clear(){
	int x, y;
	
	for(y = 0; y < 480; y ++)
		for(x = 0; x < 800; x ++)
			LCD_Point(x, y, 0X00FFFFFF);		
} 

//?��2a�䣤?t?��o����y 
//D��2?��o?T 
//����???�̡�o��???��??a����x��?y?�̡���??��11��?re_touch?D 
//1|?����o
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

//?D?????����??��o����y
//D��2?��o?T 
//����???�̡�o����??��??????����?��??��?�̡�?��?����??������?��e?a��o1234��?��???��?��????��?a0 
//1|?����o
//     1
//    ?�� 
//4?? 0 ?��2 
//   ?y 
//   3
//
//??��??����?����?����? 
//  0?��x 
// ?y
// y 
int Return_Dir(){
	struct input_event touch;
	struct re_touch re_touch = {0, 0, 0, 0, 0};
	
	//?��2a���?? 
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
	
	//?��2a?��?a 
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
	
	int rela_x, rela_y;//x��?y?��????��?	
	
	rela_x = re_touch.end_x - re_touch.star_x;
	rela_y = re_touch.end_y - re_touch.star_y;
	
	//?D??��??��
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

