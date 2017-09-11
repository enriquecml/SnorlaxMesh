#include "Helper.h"
unsigned long substract(unsigned long a,unsigned long b){
	if(b>a)
		return 0;
	return a-b;
}

unsigned long snorlax_min(unsigned long a,unsigned long b){
	if(a>b)
		return b;
	return a;
}

unsigned long snorlax_max(unsigned long a,unsigned long b){
	if(a>b)
		return a;
	return b;
}