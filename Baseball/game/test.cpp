/*
 * test.c
 *
 *  Created on: Sep 5, 2010
 *      Author: brof
 */




#include <iostream>
#include <cmath>
#include <stdlib.h>
#include <ctime>
#include <sys/time.h>

using namespace std;


/* From Quake 3 Arena
================
Sys_Milliseconds
================
*/
/* base time in seconds, that's our origin
   timeval:tv_sec is an int:
   assuming this wraps every 0x7fffffff - ~68 years since the Epoch (1970) - we're safe till 2038
   using unsigned long data type to work right with Sys_XTimeToSysTime */
unsigned long sys_timeBase = 0;
/* current time in ms, using sys_timeBase as origin
   NOTE: sys_timeBase*1000 + curtime -> ms since the Epoch
     0x7fffffff ms - ~24 days
   although timeval:tv_usec is an int, I'm not sure wether it is actually used as an unsigned int
     (which would affect the wrap period) */
long curtime;
long Sys_Milliseconds (void)
{
	struct timeval tp;

	gettimeofday(&tp, NULL);

	if (!sys_timeBase)
	{
		sys_timeBase = tp.tv_sec;
		return tp.tv_usec/1000;
	}

	curtime = (tp.tv_sec - sys_timeBase)*1000 + tp.tv_usec/1000;

	return curtime;
}

long Sys_Microseconds(void)	{
	struct timeval tp;

	gettimeofday(&tp, NULL);

	return tp.tv_usec*1;
}



float FastSqrt(float x){
	float xhalf = 0.5f * x;
	float save = x;
	int i = *(int*)&x; // store floating-point bits in integer
	i = 0x5f3759d5 - (i >> 1); // initial guess for Newton's method
	x = *(float*)&i; // convert new bits into float
	x = x*(1.5f - xhalf*x*x); // One round of Newton's method
	return (save*x);
}

timespec diff(timespec start, timespec end)
{
	timespec temp;
	if ((end.tv_nsec-start.tv_nsec)<0) {
		temp.tv_sec = end.tv_sec-start.tv_sec-1;
		temp.tv_nsec = 1000000000+end.tv_nsec-start.tv_nsec;
	} else {
		temp.tv_sec = end.tv_sec-start.tv_sec;
		temp.tv_nsec = end.tv_nsec-start.tv_nsec;
	}
	return temp;
}

typedef float vec_t;
typedef vec_t vec3_t[3];

void VectorDivide(const vec3_t a, const float divisor, vec3_t result)	{
	result[0] = a[0] / divisor;
	result[1] = a[1] / divisor;
	result[2] = a[2] / divisor;
}


void VectorFastDivide(const vec3_t a, const float divisor, vec3_t result)	{
	float fractValue = 1 / divisor;
	result[0] = a[0] * fractValue;
	result[1] = a[1] * fractValue;
	result[2] = a[2] * fractValue;
}


int main(void)	{
//	long markerTime, curTime;
	float result;
	timespec time1, time2;
//	int temp;

	clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &time1);
	result = sqrtf(7913.0);
	clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &time2);

	cout << "Square root of 7913: " << result << endl;
	cout<<diff(time1,time2).tv_sec<<":"<<diff(time1,time2).tv_nsec<<endl;



	clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &time1);
	result = FastSqrt(7913.0);
	clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &time2);

	cout << "Square root of 7913: " << result << endl;
	cout<<diff(time1,time2).tv_sec<<":"<<diff(time1,time2).tv_nsec<<endl;



	return 1;
}


