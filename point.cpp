#include "point.h"

using namespace std;

Coordinate::Coordinate()
{
	x = -1; // not initialized
	y = -1;
}

Coordinate::Coordinate(long index)
{
	set(index);
}

void Coordinate::set(long index)
{
	y = index / WIDTH;
	x = index % WIDTH;
}

Point::Point()
{
	for(int i=0; i < 2; i++)
	{
		min[i] = max[i] = center[i] = -1;
	}
	size = 0;
}

void Point::add_line(int left, int right)
{
	if(left == right)
	{
		add_point(left);
	}
	else if(size == 0)
	{
		min[0] = XVAL(left);
		max[0] = XVAL(right);
		min[1] = max[1] = YVAL(left); // should be the same column
		size += right - left;
	}
	else
	{
		if(XVAL(left) < min[0]) min[0] = XVAL(left);
		if(XVAL(right) > max[0]) max[0] = XVAL(right);
		max[1] = YVAL(left); // should just be one line and should be greater than last one
		size += right - left;
	}
}

void Point::add_point(int index)
{
	if(size == 0)
	{
		min[0] = max[0] = XVAL(index);
		min[1] = max[1] = YVAL(index); // should be the same column
	}
	else
	{
		if(XVAL(index) < min[0]) min[0] = XVAL(index);
		if(XVAL(index) > max[0]) max[0] = XVAL(index);
		max[1] = YVAL(index); // should just be one line and should be greater than last one
	}
	size++;
}

int* Point::get_center()
{
	for(int i=0; i < 2; i++)
		center[i] = min[i] + ((max[i] - min[i])/2);
	return center;
}

int Point::test_point(int index)
{
	if(size == 0) // new point
		return 1;
	if(YVAL(index) > (max[1]+1)) // should never be a break - done with point
		return -1;
	if(XVAL(index) < (min[0]-THRESHOLD) || XVAL(index) > (max[0]+THRESHOLD))
		return 0; // don't add to this point, but keep active

	return 1; // should be good, add to this point
}

int Point::test_shape()
{
	int width, height, skew;

	if(size < MIN_SIZE)  
		return -1;
	if(size > MAX_SIZE)
		return -2;

	width = max[0] - min[0];
	height = max[1] - min[1];
	skew = height - width;

	if(skew > MAX_SKEW || skew < MIN_SKEW)
		return -3;

	return 1;
}