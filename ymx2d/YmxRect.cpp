#include "stdafx.h"
#include "ymx2d.h"

YmxRect::YmxRect()
	: left(0), right(0), top(0), bottom(0)
{

}

YmxRect::YmxRect(float left, float top, float right, float bottom)
	:left(left), right(right), top(top), bottom(bottom)
{

}

bool YmxRect::Intersect(const YmxRect& rect) const
{
	if(fabs(left + right - rect.left - rect.right) < right - left + rect.right - rect.left )
		if(fabs(top + bottom - rect.top - rect.bottom) < bottom - top + rect.bottom - rect.top)
			return true;

	return false;
}

bool YmxRect::IsPointIn(float x, float y) const
{
	if(x >= left && x <= right && y >= top && y <= bottom) 
		return true;

	return false;
}