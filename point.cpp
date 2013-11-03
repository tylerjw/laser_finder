#include "point.h"
#include <stdio.h>
#include <stdlib.h>

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

int point_finder(unsigned char* working_green, int (*center_points)[2], int length)
{
	// point finder variables
	int left, right;
	const int threshold_C = 10;
	Point points[length];
	const int unused_C = 0;
	const int active_C = 1;
	const int consemated_C = 2;
	const int bad_C = -1;
	int point_status[length];
	int left_point = 0;
	int right_point = -1;
	int num_centers = 0;

	// point finder algorithm
	left = right = -1;
	for(int i=0; i<NUM_PIXELS; i++)
	{
		if(*(working_green+i) > threshold_C)
		{
			if(left == -1) // new line
			{
				left = right = i;
			}
			else
			{
				right = i;
			}
		} else if(left != -1) {
			bool found = false;
			for(int j=left_point; j<=right_point;j++)
			{
				if(point_status[j] == active_C)
				{
					int test = points[j].test_point(left);
					if(test == 1)
					{
						points[j].add_line(left,right);
						found = true;
						break;
					} else if (test == -1) {
						// point should be consemated
						if(j == left_point)
							left_point++;
						point_status[j] = consemated_C;
					}
				}
			}
			if(!found)
			{
				right_point++;
				point_status[right_point] = active_C;
				points[right_point].add_line(left, right);
			}
			left = -1;
		}
	}
	for(int i = left_point; i <= right_point; i++)
		point_status[i] = consemated_C;
	for(int i = 0; i < 100 && point_status[i] != unused_C; i++)
	{	
		int* center = points[i].get_center();
		int test = points[i].test_shape();
		if(test < 0) // bad
		{
			point_status[i] = bad_C;
			printf("%d(%d): (%d,%d) - bad (%d)\n", i, point_status[i], *center, *(center+1), test);
			continue;
		} else {
			center_points[num_centers][0] = *center;
			center_points[num_centers][1] = *(center+1);
			num_centers++;
		}
		//printf("%d(%d): (%d,%d)\n", i, point_status[i], *center, *(center+1));
	}

#ifdef DEBUG_FILE
	FILE *f = fopen("working.txt", "w");
	if (f == NULL)
	{
		printf("Error opening file!\n");
		exit(1);
	}

	fprintf(f, "/* green values only, 640 x 480 pix */ \n\n");
	for(int i=1; i < (NUM_PIXELS); i++)
	{
		bool center = false;
		if(i != 0 && (i-1)%(WIDTH) == 0)
			fprintf(f, "\n");
		for(int j = 0; j < num_centers; j++)
		{
			if(XVAL(i) == center_points[j][0] && YVAL(i) == center_points[j][1])
			{
				fprintf(f, "XXX,");
				center = true;
				break;
			}
		}
		if(!center)
			fprintf(f,"%3d,",*(working_green+i));
		center = false;
	}

	fclose(f);
#endif
	
	return num_centers;
}