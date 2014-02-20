#include "stdafx.h"
#include "ymx2d.h"

static float sinValArray[361];
static float cosValArray[361];

// This class is only for initializing the math constants
class MathInitHelper
{
public:
	MathInitHelper()
	{
		float factor = PI / 180;
		int i;
		for (i = 0; i < 361; i++)
		{
			sinValArray[i] = (float)sin(double(i * factor));
			cosValArray[i] = (float)cos(double(i * factor));
		}
	}
};

// The default constructor will be called when loading
static MathInitHelper initHelper;

YMX2D_API inline float SinFastByAngle(float angle)
{
	while(angle < 0) angle += 360;
	while(angle > 360) angle -= 360;

	int floorAngle = floor(angle);
	int ceilAngle = ceil(angle);
	if(floorAngle == ceilAngle)
		return sinValArray[ceilAngle];

	float factor = angle - floorAngle;
	return sinValArray[floorAngle] * (1- factor) + sinValArray[ceilAngle] * factor;
}

YMX2D_API inline float CosFastByAngle(float angle)
{
	while(angle < 0) angle += 360;
	while(angle > 360) angle -= 360;

	int floorAngle = floor(angle);
	int ceilAngle = ceil(angle);
	if(floorAngle == ceilAngle)
		return cosValArray[ceilAngle];

	float factor = angle - floorAngle;
	return cosValArray[floorAngle] * (1- factor) + cosValArray[ceilAngle] * factor;
}

YMX2D_API float SinFast(float rad)
{
	static float MulFactor = 180 / PI;
	float angle = rad * MulFactor;

	return SinFastByAngle(angle);
}



YMX2D_API float CosFast(float rad)
{
	static float MulFactor = 180 / PI;
	float angle = rad * MulFactor;

	return CosFastByAngle(angle);
}


YMX2D_API float GetRandomFloat(float lowBound, float highBound)
{
	if( lowBound >= highBound ) // bad input
		return lowBound;

	// get random float in [0, 1] interval
	float f = (rand() % 10000) * 0.0001f; 

	// return float in [lowBound, highBound] interval. 
	return (f * (highBound - lowBound)) + lowBound; 
}

YMX2D_API int GetRandomInteger(int lowBound, int highBound)
{
	if(lowBound >= highBound)
		return lowBound;

	return rand() % (highBound + 1 - lowBound) + lowBound;
}

void RotateVertex(YmxVertex* v, YmxVertex* pivot_v, float theta)
{
	float dx = (v->x - pivot_v->x) * CosFast(theta) - (v->y - pivot_v->y) * SinFast(theta);
	float dy = (v->x - pivot_v->x) * SinFast(theta) + (v->y - pivot_v->y) * CosFast(theta);
	v->x = dx + pivot_v->x;
	v->y = dy + pivot_v->y;
}
