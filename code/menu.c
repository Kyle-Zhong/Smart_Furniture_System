#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <pthread.h>
#include <unistd.h>
#include <linux/input.h>
#include <sys/time.h>
#include <sys/mman.h>
#include <sys/ioctl.h>
#include <malloc.h>
#include <linux/videodev2.h>
#include <termios.h>

#include <asm/types.h>        
#include <linux/videodev2.h>

#include "menu.h"
#include "LCD.h"
#include "api_v4l2.h"
#include "lcdhead.h"

#define DEVICE_NAME "/dev/s3c2410_serial3"

//ÏÔÊ¾×ÀÃæº¯Êı 
//ĞÍ²Î£ºÎŞ 
//·µ»ØÖµ£ºÎŞ 
//¹¦ÄÜ£ºË¢ĞÂ¡¢ÏÔÊ¾×ÀÃæ 
void Menu_Creat(){
	LCD_Clear();
	
	LCD_BMP("home.bmp", 0, 0);
	
	/*
	LCD_BMP("picture.bmp", 200, 100);
	LCD_BMP("music.bmp", 500, 100);
	LCD_BMP("mp4.bmp", 0, 200);
	*/
}

//ÅĞ¶Ïµã»÷ÇøÓòº¯Êı
//ĞÍ²Î£ºabs_valueÎª´¥ÃşÊı¾İ½á¹¹Ìå£¬star_x¡¢star_y¡¢end_x¡¢end_y×é³É¼ì²âÇøÓò
//·µ»ØÖµ£ºÂäÔÚ¼ì²âÇøÓòÄÚ·µ»Ø1£¬·ñÎª·µ»Ø0
//¹¦ÄÜ£º¼ì²â´¥ÃşµãÊÇ·ñÂäÔÚÖ¸¶¨ÇøÓò
int Menu_Judge(struct re_touch abs_value, int star_x, int star_y, int end_x, int end_y){
	if(star_x < abs_value.end_x && star_y < abs_value.end_y && end_x > abs_value.end_x && end_y > abs_value.end_y)
		return 1;
	else
		return 0;
}

//Ïà²áAPPº¯Êı
//ĞÍ²Î£º 
//·µ»ØÖµ£º 
//¹¦ÄÜ£º×óÓÒ»®¶¯ÇĞ»»Í¼Æ¬£¬ÉÏÏÂ»®¶¯ÍË³ö·µ»Ø²Ëµ¥½çÃæ
void Picture(){
	int dir = 0, pit_num = 1;
	char buf[20] = {0};
	
	sprintf(buf, "%d%d0.bmp", pit_num, pit_num);
	LCD_BMP(buf, 0, 0);
	
	while(1){
		dir = Return_Dir();
		
		if(dir == 2){
			pit_num ++;
			
			if(pit_num == 4)
				pit_num = 1;
			
			sprintf(buf, "%d%d0.bmp", pit_num, pit_num);
			LCD_BMP(buf, 0, 0);
		}
		if(dir == 4){
			pit_num --;
			
			if(pit_num == 0)
				pit_num = 3;
			
			sprintf(buf, "%d%d0.bmp", pit_num, pit_num);
			LCD_BMP(buf, 0, 0);
		}
		if(dir == 1 || dir == 3)
			break;
	}
}

//²¥·ÅÆ÷APPº¯Êı
//ĞÍ²Î£º 
//·µ»ØÖµ£º
//¹¦ÄÜ£º²¥·ÅÆ÷Ö´ĞĞº¯Êı
void Mp3_Player(){
	int music_num = 1, star_flag = 0;
	char buf[20] = {0};
	
	LCD_Clear();
	LCD_BMP("mp3_player.bmp", 0, 0);
	
	struct re_touch abs;
	
	while(1){
		abs = Return_ABS();
		
		if(Menu_Judge(abs, 636, 327, 756, 447)){
				sprintf(buf, "madplay %d.mp3 &", music_num);
				system(buf);
		}
		if(Menu_Judge(abs, 439, 327, 559, 447)){
			system("killall madplay");
		}
		if(Menu_Judge(abs, 45, 327, 165, 447)){
			music_num --;
			
			if(music_num == 0)
				music_num = 3;
				
			system("killall madplay");
			
			sprintf(buf, "madplay %d.mp3 &", music_num);
			system(buf);
		}
		if(Menu_Judge(abs, 242, 327, 362, 447)){
			music_num ++;
			
			if(music_num == 4)
				music_num = 1;
				
			system("killall madplay");
			
			sprintf(buf, "madplay %d.mp3 &", music_num);
			system(buf);
		}
		if(Menu_Judge(abs, 0, 0, 100, 100)){
			system("killall madplay");
			
			break;
		}
	}
}

void Mp4_Player(){
	int dir = 0;
	
	system("mplayer mv.mp4 &");
	
	while(1){
		dir = Return_Dir();
		
		if(dir == 1 || dir == 3){
			system("killall mplayer");
			
			break;
		}

	}
}

void *Monitor_Play(void *arg){
	FreamBuffer freambuf;

	lcd_open();
	linux_v4l2_device_init();
	linux_v4l2_start_capturing();
	uint8_t *data=NULL;
	
	Init_Convert_Table(); //   æ å    ‰  åˆ §‹  æ    ¼
	printf("init table\n"); 
	
	LCD_BMP("side_bar.bmp", 640, 0);
	
	while(1)
	{
		linux_v4l2_get_fream(&freambuf);
		data = YUYV_To_RGB24(freambuf.buf,640,480);
		
		lcd_draw_bmp_for_buf(0,0,data,640,480);
		free(data);
		data=NULL;
	}
}


void Monitor(){
	pthread_t monitor_play;
	int ret;
	
	ret = pthread_create(&monitor_play, NULL, Monitor_Play, NULL);
	
	if(ret != 0){
		perror("pthread_create error");
		
		return;
	}
	
	struct re_touch abs;
	
	while(1){
		abs = Return_ABS();

		if(Menu_Judge(abs, 640, 0, 800, 160)){
			linux_v4l2_stop_capturing();
			linux_v4l2_device_uinit();
			pthread_cancel(monitor_play);
			
			break;
		}
		if(Menu_Judge(abs, 640, 160, 800, 320)){
			linux_v4l2_start_capturing();
		}
		if(Menu_Judge(abs, 640, 320, 800, 480)){
			linux_v4l2_stop_capturing();
		}
	}
}

int ser_fd;

void Serial_Init(){
	struct termios termios_new;
	bzero( &termios_new, sizeof(termios_new));
	/*Ô­Ê¼Ä£Ê½*/
	cfmakeraw(&termios_new);

	/*²¨ÌØÂÊÎª115200*/
	termios_new.c_cflag=(B115200);
	termios_new.c_cflag |= CLOCAL | CREAD;
	
	/*8Î»Êı¾İÎ»*/
	termios_new.c_cflag &= ~CSIZE;
	termios_new.c_cflag |= CS8;

	/*ÎŞÆæÅ¼Ğ£ÑéÎ»*/
	termios_new.c_cflag &= ~PARENB;

	/*1Î»Í£Ö¹Î»*/
	termios_new.c_cflag &= ~CSTOPB;
	/*Çå³ı´®¿Ú»º³åÇø*/
	tcflush( ser_fd,TCIOFLUSH);
	termios_new.c_cc[VTIME] = 0;
	termios_new.c_cc[VMIN] = 4;
	tcflush ( ser_fd, TCIOFLUSH);
	/*´®¿ÚÉèÖÃÊ¹ÄÜ*/
	tcsetattr( ser_fd ,TCSANOW,&termios_new);	
}

void *Con(void *arg){
	while(1){
		write(ser_fd,"00",2);
		sleep(2);
		write(ser_fd,"01",2);
		sleep(2);
		write(ser_fd,"10",2);
		sleep(2);
		write(ser_fd,"11",2);
		sleep(2);
	}
}

void Control(){
	
	LCD_BMP("controller.bmp", 0, 0);
		char message[6];
	//´ò¿ª´®¿Ú+³õÊ¼»¯
	ser_fd = open(DEVICE_NAME,O_RDWR);
	if(ser_fd == -1)
	{
		printf("serical open failed!\n");
		return;
	}
	Serial_Init(ser_fd);
	
	if(fcntl(ser_fd,F_SETFL,0)<0)
	{
		exit(1);
	}
	tcdrain(ser_fd);   //Í¨ÓÃÖÕ¶Ë¿ØÖÆ
	tcflush( ser_fd, TCIOFLUSH); //Ë¢ĞÂ»º³åÇø
	
	pthread_t controle;
	int ret;
	
	ret = pthread_create(&controle, NULL, Con, NULL);
	
	int dir;
	
	while(1)
	{
		dir = Return_Dir();
		
		if(dir == 1 || dir == 3){
			pthread_cancel(controle);
			
			break;
		}
	}
}
