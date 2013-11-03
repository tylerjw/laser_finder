// laser_finder.c - finds the location of laser points comparing two images
#include "images.h"
#include "point.h"
#include <stdio.h>
#include <stdlib.h>

#define POINT_BUFFER_LENGTH		200
#define MAX_COLUMNS				30

extern struct image no_dots_image;
extern struct image dots_image;

int main()
{
	unsigned char working_green[NUM_PIXELS];
	int center_points[POINT_BUFFER_LENGTH][2];
	int num_points = 0;
	int num_columns = 0;
	int col_idx[MAX_COLUMNS];

	subtraction_green_filter(working_green, &dots_image, &no_dots_image);

	num_points = point_finder(working_green, center_points, POINT_BUFFER_LENGTH);

	num_columns = sort_by_col(center_points, num_points, col_idx, MAX_COLUMNS);

	printf("Points Found %d, Columns Found: %d\n", num_points, num_columns);

	for(int i=0,j=0; i<num_points; i++)
	{
		if(i == col_idx[j])
			printf("Column %d:\n", j++);
		
		printf("(%d,%d)\n", center_points[i][0], center_points[i][1]);
	}

	return 0;
}