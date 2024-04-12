#include "dht21.h"
sbit  DQ1 = P2^5;
sbit  DQ2 = P2^6;
sbit  DQ3 = P2^7;
//----------------------------------------------//
//----------------������--------------------//
//----------------------------------------------//
			

//unsigned char str[5]={"RS232"};		   
unsigned char T_zhengshu1 = 0;
unsigned char T_xiaoshu1  = 0;
unsigned char T_zhengshu2 = 0;
unsigned char T_xiaoshu2  = 0;
unsigned char T_zhengshu3 = 0;
unsigned char T_xiaoshu3  = 0;

/* ��ʼ��ds18s20 */
void Init_DS18S20(void)
{
	unsigned char x = 0;
	DQ1 = 1;     /* DQ��λ */
	DQ2 = 1;
	DQ3 = 1;
	Delayus(8);  /* ������ʱ
	DQ1 = 0;     /* ��Ƭ����DQ���� */
	DQ2 = 0;
	DQ3 = 0;
	Delayus(80); /* ��ȷ��ʱ ���� 480us */
	DQ1 = 1;     /* �������� */
	DQ2 = 1;
	DQ3 = 1;
	Delayus(14);
	x = DQ1;     /* ������ʱ�� ���x=0���ʼ���ɹ� x=1���ʼ��ʧ�� */
	x = DQ2;
	x = DQ3;
	Delayus(20);
}

/* ��һ���ֽ� */
unsigned char ReadOneChar1(void)
{
	unsigned char i = 0;
	unsigned char dat = 0;
	for (i = 8; i > 0; i--) {
		DQ1 = 0; /* �������ź� */
		dat>>=1;
		DQ1 = 1; /* �������ź� */
		if(DQ1)
		dat |= 0x80;
		Delayus(4);
	}
	return(dat);
}

/* ��һ���ֽ� */
unsigned char ReadOneChar2(void)
{
	unsigned char i = 0;
	unsigned char dat = 0;
	for (i = 8; i > 0; i--) {
		DQ2 = 0; /* �������ź� */
		dat>>=1;
		DQ2 = 1; /* �������ź� */
		if(DQ2)
		dat |= 0x80;
		Delayus(4);
	}
	return(dat);
}

/* ��һ���ֽ� */
unsigned char ReadOneChar3(void)
{
	unsigned char i = 0;
	unsigned char dat = 0;
	for (i = 8; i > 0; i--) {
		DQ3 = 0; /* �������ź� */
		dat>>=1;
		DQ3 = 1; /* �������ź� */
		if(DQ3)
		dat |= 0x80;
		Delayus(4);
	}
	return(dat);
}

/* дһ���ֽ� */
void WriteOneChar(unsigned char dat)
{
	unsigned char i = 0;
	for (i = 8; i > 0; i--) {
		DQ1 = 0;
		DQ2 = 0;
		DQ3 = 0;
		DQ1 = dat & 0x01;
		DQ2 = dat & 0x01;
		DQ3 = dat & 0x01;
		Delayus(2);
		DQ1 = 1;
		DQ2 = 1;
		DQ3 = 1;
		dat>>=1;
	}
}

/*
 * �¶ȶ�ȡ�ӳ���
 *
 * ���±�����Ϊȫ�ֱ���
 * �¶ȵ�8λ== T_zhengshu��T_xiaoshu
 */

void ReadTemperature(void)
{
//unsigned int ReadTemperature()
	unsigned char TL81 = 0;
	unsigned char TH81 = 0;
	unsigned char TL82 = 0;
	unsigned char TH82 = 0;
	unsigned char TL83 = 0;
	unsigned char TH83 = 0;
	Init_DS18S20();

	WriteOneChar(0xCC);    /* ����������кŵĲ��� */
	WriteOneChar(0x44);    /* �����¶�ת�� */
	Init_DS18S20();
	WriteOneChar(0xCC);    /* ����������кŵĲ��� */
	WriteOneChar(0xBE);    /* ��ȡ�¶ȼĴ��� */
	
	Delayus(400);
	TL81 = ReadOneChar1(); /* ����8λ */
	Delayus(400);
	//TH81=ReadOneChar1(); /* ����8λ */
	TL82 = ReadOneChar2(); /* ����8λ */
	Delayus(400);
	//TH82=ReadOneChar2(); /* ����8λ */
	TL83 = ReadOneChar3(); /* ����8λ */
	Delayus(400);
	//TH83=ReadOneChar3(); /* ����8λ */
	
	T_zhengshu1 = TL81 >> 1;
	T_xiaoshu1  = TL81 & 0x01;
	T_zhengshu2 = TL82 >> 1;
	T_xiaoshu2  = TL82 & 0x01;
	T_zhengshu3 = TL83 >> 1;
	T_xiaoshu3  = TL83 & 0x01;
}
