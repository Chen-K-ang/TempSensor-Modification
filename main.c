#include <reg52.h>
//sfr WDT_CONTR=0xe1;//����STC��Ƭ�����¼���Ŀ��Ź��Ĵ���
#include <intrins.h>
#include <stdio.h>
#include "dht21.h"
//#define WEIGOU WDT_CONTR=0x30 //���Ź��������ú͡�ι�������������Ź����ʱ��Ϊ 71.1ms

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

unsigned long time_20ms = 0;	/* ��ʱ���� */
unsigned char dis0[8];
unsigned char dis1[8];
unsigned char dis2[10];
unsigned char dis3[8];
unsigned char dis4[8];
unsigned char read_DS18S20 = 0;	/* ��ȡ��־ */
char new_temp1, new_temp2, new_temp3;
char temp1, temp2, temp3;
char t1, t2, t3;

unsigned char code RST[]  = "AT+RST\r\n"; /*����ģ�� */
unsigned char code LYMS[] = "AT+CWMODE=1\r\n";  /* ����WIFIģ��Ϊstationģʽ */
unsigned char code SZLY[] = "AT+CWJAP=\"USR_G805_0379\",\"www.usr.cn\"\r\n"; /*����WIFIģ������WIFI������������*/
unsigned char code KQFU[] = "AT+CIPSTART=\"TCP\",\"47f5m32219.qicp.vip\",58379\r\n"; /* ���ӷ����� */
unsigned char code TCMS[] = "AT+CIPMODE=1\r\n"; /*͸��ģʽ */
unsigned char code FSSJ[] = "AT+CIPSEND\r\n";   /*�������� */

unsigned char read_ad(unsigned char control);
unsigned char read_byte();
void Init_Timer0(void);
void UART_Init(void);
void SendByte(unsigned char dat);			 /* ���ڷ���1���ַ� */
void SendStr(unsigned char *s, unsigned char length);	 /* ���ڷ����ַ��� */
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
//	WEIGOU;         /* ������һ�����ÿ��Ź���ʱ������������ */
//	Init_Timer0();  /* ��ʱ��0��ʼ�� */
	init_iic();
	UART_Init();    /* ���ڳ�ʼ�� */
	Delayms(2000);
	
	jidian_init();
	Delayms(1000);
	
	ReadTemperature();
	filter();
	Init_Timer0();  /* ��ʱ��0��ʼ�� */
//	led1=0;
//	read_ad1();
	
//	led1=1;

	/* ��ѭ�� */
	while(1) {	
		if(read_DS18S20 == 1) {												  
			read_DS18S20 = 0;
			ET0 = 0;                 /* �رն�ʱ���ж� */
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

			/* �������¶�T4=7�� */
			if (t3 > 7) {
				if (t1 <= 5) {
					/* ���ݿͻ�Ҫ�����������ֵTS1=5�� */
					jidian_command(0, 1, 1, 1);
				} else if (t1 <= 55) {
					/* ���ݿͻ�Ҫ�����������ֵTS1=55�� */
					jidian_command(1, 1, 0, 1);
				} else {
					jidian_command(1, 1, 0, 0);
				}
			} else {
				jidian_command(0, 0, 1, 1);
			}
			
			/* ���ݿͻ�Ҫ��ʪ��������ֵTS1=55�� */		
			if(tempshidu2>55) {
				if (t3 > (7 + 2)) {
					/* ���������¶�T4Ϊ7�棬�ҿͻ����������¶�ƫ����ֵΪ2�� */
					if (tempshidu2 > tempshidu1) {
						/* �ͻ�����ʪ��������ֵΪ55% */
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
				/* ���������¶�T4Ϊ7�棬�ҿͻ����������¶�ƫ����ֵΪ2�� */
				
				if(t1 < 55) {
					/* �ͻ���������������ֵΪ55�� */
					jidian_command(1, 1, 1, 1);
				} else {
					jidian_command(1, 1, 0, 0);
				}
			} else if (tempshidu2 < tempshidu1) {
				jidian_command(1, 0, 1, 1);
			} else if (t2 < (1 + 2)) {
				/* ���������¶�T5Ϊ1�棬�ҿͻ����������¶�ƫ����ֵΪ2�� */
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
			SendStr(dis1, 8);			 //����
			SendStr(dis2, 10);
			SendStr(dis3, 8);
			SendStr(dis4, 8);
		//	SendStr("s",1);
			SendStr("restart collecting...\n",23);
			ET0 = 1;	 //����ʱ���ж�
		}

	}
}

void Init_Timer0(void)
{
	TMOD |= 0x01;	          /* ʹ��ģʽ1��16λ��ʱ����ʹ��"|"���ſ�����ʹ�ö����ʱ��ʱ����Ӱ�� */		     
	TH0= (65536-20000) / 256; /* ���¸�ֵ 20ms */
	TL0= (65536-20000) % 256;
	EA  = 1;                  /* ���жϴ� */
	ET0 = 1;                  /* ��ʱ���жϴ� */
	TR0 = 1;                  /* ��ʱ�����ش� */
}

void Timer0_isr(void) interrupt 1 
{
//	TH0=(65536-20000)/256;		  /* ���¸�ֵ 20ms */
//	TL0=(65536-20000)%256;
	TL0 = 0x00;		
	TH0 = 0xB8;			
	time_20ms++;
	
	if (time_20ms % 3000 == 0) {
		/* ��ʱ�������� */
		read_DS18S20 = 1;
	}
}

void UART_Init(void)
{
	SCON  = 0x50;	/* SCON: ģʽ 1, 8-bit UART, ʹ�ܽ��� */  
	TMOD |= 0x20;       /* TMOD: timer 1, mode 2, 8-bit ��װ */
	TH1   = 0xFD;       /* TH1:  ��װֵ 9600 ������ ���� 11.0592MHz */
	TR1   = 1;          /* TR1:  timer 1 �� */                       
	EA    = 1;          /* �����ж� */
	ES    = 1;          /* �򿪴����ж� */
}

/* ���ڷ��͵��ֽ����� */
void SendByte(unsigned char dat)
{
	unsigned char time_out;
	time_out = 0x00;
	SBUF = dat; /* �����ݷ���SBUF�� */
	
	/* ����Ƿ��ͳ�ȥ */
	while((!TI) && (time_out < 100)) {
		time_out++;
		Delayus(20); /* δ���ͳ�ȥ ���ж�����ʱ */
	}	
	TI = 0;	/* ���TI��־ */
}

/* ���Ͷ������ַ��� */
void SendStr(unsigned char *s, unsigned char length)	   
{
	unsigned char NUM;
	NUM = 0x00;
	
	/* ���ͳ��ȶԱ� */
	while(NUM < length) {
		SendByte(*s); /* ���͵��ֽ����� */
		s++;          /* ָ��++ */
		NUM++;        /* ��һ��++ */
  	 }
}

/* �����жϷ������ */
void UART_SER (void) interrupt 4
{
	/* �ж��ǽ����жϲ��� */
	if (RI) {
		RI = 0;  /* ��־λ���� */
	}
	
	/* ����Ƿ��ͱ�־λ������ */
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

/* Ӧ���ź� */
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
//	Delayms(1500);     /* �ȴ�WIFI�ϵ��ʼ�� */
//	SendStr(LYMS, 13); /* AT+CWMODE=1 ����stationģʽ��ֻ�����������ȵ� */
//	Delayms(1500);
//	SendStr(SZLY, 46); /* ����WIFIģ������WIFI������������ */
//	Delayms(15000);    /* �ȴ�����WIFI�Լ���ȡIP��ַ */
//	SendStr(KQFU, 55); /* ���ӷ����� */
//	Delayms(15000);    /* �ȴ����ӳɹ� */
//	SendStr(TCMS, 14); /* ����Ϊ͸��ģʽ */
//	Delayms(5000);
//	SendStr(FSSJ, 12); /* ׼���������� */
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
	 * XXX : �� return t1������
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

