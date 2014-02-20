#include "stdafx.h"
#include "ymx2d.h"

YmxVector::YmxVector()
{
	x = y = 0;
}

YmxVector::YmxVector(float x, float y):x(x),y(y)
{

}


float YmxVector::GetLength()
{
	return (float)sqrt(x * x + y * y);
}

void YmxVector::Normalize()
{
	float lengthRecip = 1.0f / (float)sqrt(x * x + y * y);
	x = x * lengthRecip;
	y = y * lengthRecip;
}

float YmxVector::GetLengthSq()
{
	return x * x + y * y;
}

YmxVector YmxVector::operator +(const YmxVector& v) const
{
	return YmxVector(x + v.x, y + v.y);
}

YmxVector YmxVector::operator -(const YmxVector& v) const
{
	return YmxVector(x - v.x, y - v.y);
}

YmxVector YmxVector::operator *(float a) const
{
	return YmxVector(x * a, y * a);
}

YmxVector YmxVector::operator -() const
{
	return YmxVector(-x, -y);
}

float YmxVector::operator*(const YmxVector& v) const
{
	return x * v.x + y * v.y;
}

YmxVector& YmxVector::operator +=(const YmxVector& v)
{
	x += v.x;
	y += v.y;
	return *this;
}

YmxVector& YmxVector::operator -=(const YmxVector& v)
{
	x -= v.x;
	y -= v.y;
	return *this;
}

YmxVector& YmxVector::operator *=(float a)
{
	x *= a;
	y *= a;
	return *this;
}

bool YmxVector::operator==(const YmxVector& v) const
{
	return x == v.x && y == v.y;
}

bool YmxVector::operator !=(const YmxVector& v) const
{
	return x != v.x || y != v.y;
}