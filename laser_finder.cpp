// laser_finder.c - finds the location of laser points comparing two images
#include "images.h"
#include "point.h"
#include <stdio.h>
#include <stdlib.h>

#define POINT_BUFFER_LENGTH		200

extern struct image no_dots_image;
extern struct image dots_image;

int main()
{
	unsigned char working_green[NUM_PIXELS];
	int center_points[POINT_BUFFER_LENGTH][2];
	int num_points = 0;

	subtraction_green_filter(working_green, &dots_image, &no_dots_image);

	num_points = point_finder(working_green, center_points, POINT_BUFFER_LENGTH);

	printf("Points Found %d\n", num_points);
	for(int i=0; i<num_points; i++)
		printf("(%d,%d)\n", center_points[i][0], center_points[i][1]);

	return 0;
}