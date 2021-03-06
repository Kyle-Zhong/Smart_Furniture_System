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

//显示桌面函数 
//型参：无 
//返回值：无 
//功能：刷新、显示桌面 
void Menu_Creat(){
	LCD_Clear();
	
	LCD_BMP("home.bmp", 0, 0);
	
	/*
	LCD_BMP("picture.bmp", 200, 100);
	LCD_BMP("music.bmp", 500, 100);
	LCD_BMP("mp4.bmp", 0, 200);
	*/
}

//判断点击区域函数
//型参：abs_value为触摸数据结构体，star_x、star_y、end_x、end_y组成检测区域
//返回值：落在检测区域内返回1，否为返回0
//功能：检测触摸点是否落在指定区域
int Menu_Judge(struct re_touch abs_value, int star_x, int star_y, int end_x, int end_y){
	if(star_x < abs_value.end_x && star_y < abs_value.end_y && end_x > abs_value.end_x && end_y > abs_value.end_y)
		return 1;
	else
		return 0;
}

//相册APP函数
//型参： 
//返回值： 
//功能：左右划动切换图片，上下划动退出返回菜单界面
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

//播放器APP函数
//型参： 
//返回值：
//功能：播放器执行函数
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
	
	Init_Convert_Table(); //   鎻 愬    墠  鍒   鏍   �
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
	/*原始模式*/
	cfmakeraw(&termios_new);

	/*波特率为115200*/
	termios_new.c_cflag=(B115200);
	termios_new.c_cflag |= CLOCAL | CREAD;
	
	/*8位数据位*/
	termios_new.c_cflag &= ~CSIZE;
	termios_new.c_cflag |= CS8;

	/*无奇偶校验位*/
	termios_new.c_cflag &= ~PARENB;

	/*1位停止位*/
	termios_new.c_cflag &= ~CSTOPB;
	/*清除串口缓冲区*/
	tcflush( ser_fd,TCIOFLUSH);
	termios_new.c_cc[VTIME] = 0;
	termios_new.c_cc[VMIN] = 4;
	tcflush ( ser_fd, TCIOFLUSH);
	/*串口设置使能*/
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
	//打开串口+初始化
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
	tcdrain(ser_fd);   //通用终端控制
	tcflush( ser_fd, TCIOFLUSH); //刷新缓冲区
	
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
