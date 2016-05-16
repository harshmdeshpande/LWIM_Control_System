#ifndef TIMER_H_
#define TIMER_H_

#include <windows.h>

typedef struct {
    LARGE_INTEGER start;
    LARGE_INTEGER stop;
} timerStruct;

class Timer
{
public:
	Timer();
	
	void startTimer( );
	void stopTimer( );
	double getElapsedTime();
	
	virtual ~Timer();
	
private:
	timerStruct timer;
	LARGE_INTEGER frequency;
	double LIToSecs( LARGE_INTEGER & L);
};

#endif /*TIMER_H_*/
