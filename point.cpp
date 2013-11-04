#include "point.h"
#include <stdio.h>
#include <stdlib.h>

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
			// printf("%d(%d): (%d,%d) - bad (%d)\n", i, point_status[i], *center, *(center+1), test);
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

int sort_by_col(int (*center_points)[2], int num_points, int* col_idx, int col_idx_size)
{
	int working_array[num_points][2];	// array for copying data points
	int column_number[num_points];		// the column number of each point
	int column_max[30];					// the xvalue of the point that's lowest in the column (allong y axis) maximum (+threshold)
	int column_min[30];					// the min x value, -1 once copied
	int num_col = 0;					// the number of columns found
	int const done_C = -1000;			// done with this column
	int point_count = 0;					// counter for adding values back into the array
	int col_idx_count = 0;				// counter for adding values to col_idx

	column_number[0] = num_col; // first element
	column_min[0] = center_points[0][0] - COL_THRESHOLD;
	column_max[0] = center_points[0][0] + COL_THRESHOLD;
	num_col++;
	working_array[0][0] = center_points[0][0];
	working_array[0][1] = center_points[0][1];

	for(int i = 1; i < num_points; i++) // i - point number
	{
		bool column_found = false;
		for(int j = 0; j < num_col; j++) // j - column number
		{
			if(center_points[i][0] <= column_max[j] && center_points[i][0] >= column_min[j])
			{
				// in this column
				column_number[i] = j;
				column_min[j] = center_points[i][0] - COL_THRESHOLD;
				column_max[j] = center_points[i][0] + COL_THRESHOLD;
				column_found = true;
			}
		}
		if(!column_found)
		{
			column_number[i] = num_col;
			column_min[num_col] = center_points[i][0] - COL_THRESHOLD;
			column_max[num_col] = center_points[i][0] + COL_THRESHOLD;
			num_col++;
		}
		column_found = false;

		working_array[i][0] = center_points[i][0]; // copy into working array 
		working_array[i][1] = center_points[i][1];
	}

	// sort the columns and the array
	for(int i = 0; i < num_col; i++) // i - column number
	{
		int lowest = -1;
		for(int j = 0; j < num_col; j++) // j = column number
		{
			if(lowest == -1 && column_min[j] != done_C)
			{
				lowest = j;
			}
			else if(column_max[lowest] > column_max[j] && column_min[j] != done_C)
			{
				lowest = j;
			}
		}
		column_min[lowest] = done_C;
		for(int j = 0; j < num_points; j++) // j - point number
		{
			if(column_number[j] == lowest)
			{
				center_points[point_count][0] = working_array[j][0];
				center_points[point_count][1] = working_array[j][1];
				point_count++;
			}
		}
		if(col_idx_count == 0)
			col_idx[col_idx_count++] = 0;
		if(col_idx_count < col_idx_size) // protect the array (shoud never be false)
			col_idx[col_idx_count++] = point_count; // store the values of where the columns start
	} 

	return num_col;
}



