#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <linux/input.h>
#include <pthread.h>

#include "LCD.h"
#include "menu.h"

int main(){
	LCD_Touch_Init();//触摸初始化 
	LCD_BMP_Init();//LCD显示图像初始化 
	
	struct re_touch abs;
	
	while(1){
		Menu_Creat();
		
		abs = Return_ABS();
		
		if(Menu_Judge(abs, 517, 240, 717, 320)){
			printf("picture\n");
			
			Picture();
		}
		if(Menu_Judge(abs, 303, 240, 503, 320)){
			printf("mp3 player\n");
			
			Mp3_Player();
		}
		if(Menu_Judge(abs, 88, 240, 288, 320)){
			printf("mp4 player\n");
			
			Mp4_Player();
		}
		if(Menu_Judge(abs, 517, 340, 717, 420)){
			printf("monitor\n");
			
			Monitor();
		}
		if(Menu_Judge(abs, 87, 340, 287, 420)){
			printf("control\n");
			
			Control();
		}
	}
	
	return 0;
}
