#ifndef __TASK_H__
#define __TASK_H__

class Task{

public:
	virtual void execute()=0;
	unsigned long duration_ms;
private:
	
};
#endif
