#include <reg52.h>
//sfr WDT_CONTR=0xe1;//定义STC单片机中新加入的看门狗寄存器
#include <intrins.h>
#include <stdio.h>
#include "dht21.h"
//#define WEIGOU WDT_CONTR=0x30 //看门狗启动设置和“喂狗”操作，看门狗溢出时间为 71.1ms

sbit sda = P2^0;
sbit scl = P2^1;

sbit jidian1 = P0^0;
sbit jidian2 = P0^1;
sbit jidian3 = P0^2;
sbit jidian4 = P0^3;
sbit jidian5 = P0^4;
sbit jidian6 = P0^5;

sbit led1 = P1^0;
sbit led2 = P1^1;
sbit led3 = P1^2;
sbit led4 = P1^3;
sbit led5 = P1^4;

unsigned long time_20ms = 0;	/* 定时变量 */
unsigned char dis0[8];
unsigned char dis1[8];
unsigned char dis2[10];
unsigned char dis3[8];
unsigned char dis4[8];
unsigned char read_DS18S20 = 0;	/* 读取标志 */
char new_temp1, new_temp2, new_temp3;
char temp1, temp2, temp3;
char t1, t2, t3;

unsigned char code RST[]  = "AT+RST\r\n"; /*重启模块 */
unsigned char code LYMS[] = "AT+CWMODE=1\r\n";  /* 设置WIFI模块为station模式 */
unsigned char code SZLY[] = "AT+CWJAP=\"USR_G805_0379\",\"www.usr.cn\"\r\n"; /*设置WIFI模块连接WIFI的名称与密码*/
unsigned char code KQFU[] = "AT+CIPSTART=\"TCP\",\"47f5m32219.qicp.vip\",58379\r\n"; /* 连接服务器 */
unsigned char code TCMS[] = "AT+CIPMODE=1\r\n"; /*透传模式 */
unsigned char code FSSJ[] = "AT+CIPSEND\r\n";   /*发送数据 */

unsigned char read_ad(unsigned char control);
unsigned char read_byte();
void Init_Timer0(void);
void UART_Init(void);
void SendByte(unsigned char dat);			 /* 串口发送1个字符 */
void SendStr(unsigned char *s, unsigned char length);	 /* 串口发送字符串 */
//void Online(void);
char filter();
void jidian_init(void);
void jidian_command(char a, char b, char c, char d);
//void read_ad1(void);
//unsigned char read_ad2(void);

void main(void)
{	unsigned char shidu1;
	unsigned char shidu2;
	unsigned int tempshidu1;
	unsigned int tempshidu2;
//	WEIGOU;         /* 上来第一步设置看门狗定时器，并且启动 */
//	Init_Timer0();  /* 定时器0初始化 */
	init_iic();
	UART_Init();    /* 串口初始化 */
	Delayms(2000);
	
	jidian_init();
	Delayms(1000);
	
	ReadTemperature();
	filter();
	Init_Timer0();  /* 定时器0初始化 */
//	led1=0;
//	read_ad1();
	
//	led1=1;

	/* 主循环 */
	while(1) {	
		if(read_DS18S20 == 1) {												  
			read_DS18S20 = 0;
			ET0 = 0;                 /* 关闭定时器中断 */
			ReadTemperature();
			filter();
			shidu1 = read_ad(0x00);
			Delayms(5);		 /* rh1 */
//          		shidu1=read_ad2();	
			shidu2 = read_ad(0x01);
			Delayms(5);		 /* rh2 */
			tempshidu1 = (((int)shidu1) * 2 - 80) / 3;
			Delayms(5);
			tempshidu2 = (((int)shidu2) * 2 - 80) / 3;
			Delayms(5);

			/* 记凝点温度T4=7℃ */
			if (t3 > 7) {
				if (t1 <= 5) {
					/* 根据客户要求低温启动阈值TS1=5℃ */
					jidian_command(0, 1, 1, 1);
				} else if (t1 <= 55) {
					/* 根据客户要求高温启动阈值TS1=55℃ */
					jidian_command(1, 1, 0, 1);
				} else {
					jidian_command(1, 1, 0, 0);
				}
			} else {
				jidian_command(0, 0, 1, 1);
			}
			
			/* 根据客户要求湿度启动阈值TS1=55℃ */		
			if(tempshidu2>55) {
				if (t3 > (7 + 2)) {
					/* 假设凝点温度T4为7℃，且客户给定凝点温度偏差阈值为2℃ */
					if (tempshidu2 > tempshidu1) {
						/* 客户给定湿度启动阈值为55% */
						if(tempshidu1 > 55) {
							jidian_command(1, 0, 1, 1);
						} else {
							jidian_command(1, 0, 0, 0);
						}
					} else {
						jidian_command(1, 0, 1, 1);
					}
				} else {
					jidian_command(1, 0, 1, 1);
				}
			} else if (t3 > (7 + 2))  {
				/* 假设凝点温度T4为7℃，且客户给定凝点温度偏差阈值为2℃ */
				
				if(t1 < 55) {
					/* 客户给定高温启动阈值为55℃ */
					jidian_command(1, 1, 1, 1);
				} else {
					jidian_command(1, 1, 0, 0);
				}
			} else if (tempshidu2 < tempshidu1) {
				jidian_command(1, 0, 1, 1);
			} else if (t2 < (1 + 2)) {
				/* 假设凝点温度T5为1℃，且客户给定凝点温度偏差阈值为2℃ */
				jidian_command(1, 0, 1, 1);
			} else {
				jidian_command(1, 0, 0, 0);
			}
			
			sprintf(dis3,"R1%02d.%01d", tempshidu2);
			sprintf(dis4,"R2%02d.%01d", tempshidu1);
			sprintf(dis0,"T1%02d.%01d", (int)t1, 5 * (int)T_xiaoshu1);
			sprintf(dis1,"T2%02d.%01d", (int)t2, 5 * (int)T_xiaoshu2);
			sprintf(dis2,"T3%02d.%01d", (int)t3, 5 * (int)T_xiaoshu3);
			SendStr(dis0, 8);
			SendStr(dis1, 8);			 //发送
			SendStr(dis2, 10);
			SendStr(dis3, 8);
			SendStr(dis4, 8);
		//	SendStr("s",1);
			SendStr("restart collecting...\n",23);
			ET0 = 1;	 //开定时器中断
		}

	}
}

void Init_Timer0(void)
{
	TMOD |= 0x01;	          /* 使用模式1，16位定时器，使用"|"符号可以在使用多个定时器时不受影响 */		     
	TH0= (65536-20000) / 256; /* 重新赋值 20ms */
	TL0= (65536-20000) % 256;
	EA  = 1;                  /* 总中断打开 */
	ET0 = 1;                  /* 定时器中断打开 */
	TR0 = 1;                  /* 定时器开关打开 */
}

void Timer0_isr(void) interrupt 1 
{
//	TH0=(65536-20000)/256;		  /* 重新赋值 20ms */
//	TL0=(65536-20000)%256;
	TL0 = 0x00;		
	TH0 = 0xB8;			
	time_20ms++;
	
	if (time_20ms % 3000 == 0) {
		/* 定时发送数据 */
		read_DS18S20 = 1;
	}
}

void UART_Init(void)
{
	SCON  = 0x50;	/* SCON: 模式 1, 8-bit UART, 使能接收 */  
	TMOD |= 0x20;       /* TMOD: timer 1, mode 2, 8-bit 重装 */
	TH1   = 0xFD;       /* TH1:  重装值 9600 波特率 晶振 11.0592MHz */
	TR1   = 1;          /* TR1:  timer 1 打开 */                       
	EA    = 1;          /* 打开总中断 */
	ES    = 1;          /* 打开串口中断 */
}

/* 串口发送单字节数据 */
void SendByte(unsigned char dat)
{
	unsigned char time_out;
	time_out = 0x00;
	SBUF = dat; /* 将数据放入SBUF中 */
	
	/* 检测是否发送出去 */
	while((!TI) && (time_out < 100)) {
		time_out++;
		Delayus(20); /* 未发送出去 进行短暂延时 */
	}	
	TI = 0;	/* 清除TI标志 */
}

/* 发送定长度字符串 */
void SendStr(unsigned char *s, unsigned char length)	   
{
	unsigned char NUM;
	NUM = 0x00;
	
	/* 发送长度对比 */
	while(NUM < length) {
		SendByte(*s); /* 发送单字节数据 */
		s++;          /* 指针++ */
		NUM++;        /* 下一个++ */
  	 }
}

/* 串行中断服务程序 */
void UART_SER (void) interrupt 4
{
	/* 判断是接收中断产生 */
	if (RI) {
		RI = 0;  /* 标志位清零 */
	}
	
	/* 如果是发送标志位，清零 */
	if (TI) {  
		TI = 0;
	}
}

void start_iic()
{	
//	sda=1;
	Delayus(5);
	scl = 1;
	Delayus(5);
	sda = 0;
	Delayus(5);
	scl = 0;
	Delayus(5);
}

void stop_iic()
{	
//	scl=0;
	sda = 0;
	Delayus(5);
	scl = 1;		 
	Delayus(5);
	sda = 1;
	Delayus(5);
	scl = 0;

}

void respons_iic()
{	
	unsigned char i;
	scl = 1;
	Delayus(5);	 
	while ((sda == 1) && (i < 350)) {
		i++;
	}
	sda = 0;
	Delayus(5);
}

void init_iic()
{
	sda = 1;
	Delayus(5);
	scl = 1;
	Delayus(5);			
}

void write_byte(unsigned char date)
{
	unsigned char i,temp;
	temp = date;
	for(i = 0; i < 8; i++) {
		temp = temp << 1;
		scl = 0;
		Delayus(5);
		sda = CY;
		Delayus(5);
		scl = 1;
		Delayus(5);
	}
	scl = 0;
	Delayus(5);
	sda = 1;
	Delayus(5);		
} 

unsigned char read_byte()
{
	unsigned char i, k;
	scl = 0;
	Delayus(5);
	sda = 1;
	Delayus(5);
	for (i = 0;i < 8; i++) {
		scl = 1;
		Delayus(5);
		k = (k << 1) | sda;
		scl = 0;
		Delayus(5);
	}
	
	return k;
}

/* 应答信号 */
void iicAck(void)		
{
	scl = 0;
	Delayus(5);
	sda = 0;
	Delayus(5);
	scl = 1;
	Delayus(5);
	scl = 0; 		
}

unsigned char read_ad(unsigned char control)
{
	unsigned char date;
	start_iic();
	write_byte(0x90);
	respons_iic();
	
	write_byte(control);
	respons_iic();
	stop_iic();
	
	start_iic();
	write_byte(0x91);
	respons_iic();
	
	date = read_byte();
	iicAck();
	stop_iic();
	
	return date;
}

//void Online(void)
//{
//	Delayms(1500);     /* 等待WIFI上电初始化 */
//	SendStr(LYMS, 13); /* AT+CWMODE=1 设置station模式，只能连接无线热点 */
//	Delayms(1500);
//	SendStr(SZLY, 46); /* 设置WIFI模块连接WIFI的名称与密码 */
//	Delayms(15000);    /* 等待连接WIFI以及获取IP地址 */
//	SendStr(KQFU, 55); /* 连接服务器 */
//	Delayms(15000);    /* 等待连接成功 */
//	SendStr(TCMS, 14); /* 设置为透传模式 */
//	Delayms(5000);
//	SendStr(FSSJ, 12); /* 准备发送数据 */
//	Delayms(3000);
//}
char filter()
{
	ReadTemperature();
	temp1 = T_zhengshu1;
	temp2 = T_zhengshu2;
	temp3 = T_zhengshu3;
	ReadTemperature();
	new_temp1 = T_zhengshu1;
	new_temp2 = T_zhengshu2;
	new_temp3 = T_zhengshu3;

	if ((temp1 - new_temp1 > 3) || (new_temp1 - temp1 > 3)) {
		t1 = temp1;
	}
	t1 = new_temp1;
	return t1;	
	
	/*
	 * XXX : 在 return t1处返回
	 */
//	if ((temp2 - new_temp2 > 3) || (new_temp2 - temp2 > 3)) {
//		t2 = temp2;
//	}
//	t2 = new_temp2;
//	return t2;	
//	
//	if ((temp3 - new_temp3 > 3) || (new_temp3 - temp3 > 3)) {
//		t3 = temp3;
//	}
//	t3 = new_temp3;
//	return t3;	
}

void jidian_init(void)
{
	jidian1 = 1;
	jidian2 = 1;
	jidian3 = 1;
	jidian4 = 1;
	jidian5 = 1;										
	jidian6 = 1;
}

void jidian_command(char a, char b, char c, char d)
{
	jidian5 = a;
	jidian4 = b;
	jidian3 = c;
	jidian6 = d;
}

