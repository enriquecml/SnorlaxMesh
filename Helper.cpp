#include "Helper.h"
unsigned long substract(unsigned long a,unsigned long b){
	if(b>a)
		return 0;
	return a-b;
}

unsigned long min(unsigned long a,unsigned long b){
	if(a>b)
		return b;
	return a;
}

unsigned long max(unsigned long a,unsigned long b){
	if(a>b)
		return a;
	return b;
}