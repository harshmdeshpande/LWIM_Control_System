#include "Timer.h"
#include "iostream"

Timer::Timer()
{
	timer.start.QuadPart=0;
	timer.stop.QuadPart=0;	
	QueryPerformanceFrequency( &frequency );
}

double Timer::LIToSecs( LARGE_INTEGER & L) {
	return ((double)L.QuadPart /(double)frequency.QuadPart);
}

void Timer::startTimer( ) {
    QueryPerformanceCounter(&timer.start);
}

void Timer::stopTimer( ) {
    QueryPerformanceCounter(&timer.stop);
}

double Timer::getElapsedTime() {
	LARGE_INTEGER time;
	QueryPerformanceCounter(&timer.stop);
	time.QuadPart = timer.stop.QuadPart - timer.start.QuadPart;
	//std::cout << "TIME " << LIToSecs(time) << std::endl;
    return LIToSecs( time) ;
}

Timer::~Timer()
{
}
