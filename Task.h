#ifndef __TASK_H__
#define __TASK_H__

#include <Arduino.h>

#define DEBUG 1
class Task{

public:
	Task();
	void execute();
	unsigned long duration_ms;
private:
	
};
#endif
