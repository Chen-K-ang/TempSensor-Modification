#include "dht21.h"
sbit  DQ1 = P2^5;
sbit  DQ2 = P2^6;
sbit  DQ3 = P2^7;
//----------------------------------------------//
//----------------定义区--------------------//
//----------------------------------------------//
			

//unsigned char str[5]={"RS232"};		   
unsigned char T_zhengshu1 = 0;
unsigned char T_xiaoshu1  = 0;
unsigned char T_zhengshu2 = 0;
unsigned char T_xiaoshu2  = 0;
unsigned char T_zhengshu3 = 0;
unsigned char T_xiaoshu3  = 0;

/* 初始化ds18s20 */
void Init_DS18S20(void)
{
	unsigned char x = 0;
	DQ1 = 1;     /* DQ复位 */
	DQ2 = 1;
	DQ3 = 1;
	Delayus(8);  /* 稍做延时
	DQ1 = 0;     /* 单片机将DQ拉低 */
	DQ2 = 0;
	DQ3 = 0;
	Delayus(80); /* 精确延时 大于 480us */
	DQ1 = 1;     /* 拉高总线 */
	DQ2 = 1;
	DQ3 = 1;
	Delayus(14);
	x = DQ1;     /* 稍做延时后 如果x=0则初始化成功 x=1则初始化失败 */
	x = DQ2;
	x = DQ3;
	Delayus(20);
}

/* 读一个字节 */
unsigned char ReadOneChar1(void)
{
	unsigned char i = 0;
	unsigned char dat = 0;
	for (i = 8; i > 0; i--) {
		DQ1 = 0; /* 给脉冲信号 */
		dat>>=1;
		DQ1 = 1; /* 给脉冲信号 */
		if(DQ1)
		dat |= 0x80;
		Delayus(4);
	}
	return(dat);
}

/* 读一个字节 */
unsigned char ReadOneChar2(void)
{
	unsigned char i = 0;
	unsigned char dat = 0;
	for (i = 8; i > 0; i--) {
		DQ2 = 0; /* 给脉冲信号 */
		dat>>=1;
		DQ2 = 1; /* 给脉冲信号 */
		if(DQ2)
		dat |= 0x80;
		Delayus(4);
	}
	return(dat);
}

/* 读一个字节 */
unsigned char ReadOneChar3(void)
{
	unsigned char i = 0;
	unsigned char dat = 0;
	for (i = 8; i > 0; i--) {
		DQ3 = 0; /* 给脉冲信号 */
		dat>>=1;
		DQ3 = 1; /* 给脉冲信号 */
		if(DQ3)
		dat |= 0x80;
		Delayus(4);
	}
	return(dat);
}

/* 写一个字节 */
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
 * 温度读取子程序
 *
 * 以下变量均为全局变量
 * 温度低8位== T_zhengshu，T_xiaoshu
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

	WriteOneChar(0xCC);    /* 跳过读序号列号的操作 */
	WriteOneChar(0x44);    /* 启动温度转换 */
	Init_DS18S20();
	WriteOneChar(0xCC);    /* 跳过读序号列号的操作 */
	WriteOneChar(0xBE);    /* 读取温度寄存器 */
	
	Delayus(400);
	TL81 = ReadOneChar1(); /* 读低8位 */
	Delayus(400);
	//TH81=ReadOneChar1(); /* 读高8位 */
	TL82 = ReadOneChar2(); /* 读低8位 */
	Delayus(400);
	//TH82=ReadOneChar2(); /* 读高8位 */
	TL83 = ReadOneChar3(); /* 读低8位 */
	Delayus(400);
	//TH83=ReadOneChar3(); /* 读高8位 */
	
	T_zhengshu1 = TL81 >> 1;
	T_xiaoshu1  = TL81 & 0x01;
	T_zhengshu2 = TL82 >> 1;
	T_xiaoshu2  = TL82 & 0x01;
	T_zhengshu3 = TL83 >> 1;
	T_xiaoshu3  = TL83 & 0x01;
}
