#include "delay.h"

void Delayus(int num)
{
	while(num --);
}

void Delayms(unsigned int di) 
{
	unsigned int da, db;
 	for (da = 0; da < di; da++) {
   		for(db = 0; db < 100; db++);
	}
}