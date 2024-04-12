#include <reg52.h> 

extern unsigned char T_zhengshu1, T_xiaoshu1, T_zhengshu2, T_xiaoshu2, T_zhengshu3, T_xiaoshu3;

void Delayus(int num);
void Delayms(unsigned int di);

void ReadTemperature(void);
void init_iic(void);
