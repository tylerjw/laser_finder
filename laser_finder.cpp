// laser_finder.c - finds the location of laser points comparing two images
#include "images.h"
#include "point.h"
#include <stdio.h>
#include <stdlib.h>

#define WIDTH 		640
#define HEIGHT		480
#define NUM_PIXELS	(WIDTH*HEIGHT)
#define IMAGE_SIZE	(NUM_PIXELS*3)

extern struct image no_dots_image;
extern struct image dots_image;

/*
	output = dots - no_dots
*/
struct image* subtraction_filter(struct image* output, struct image* dots, struct image* no_dots)
{
	int i;
	for(i=0; i < (IMAGE_SIZE); i++)
	{
		if(no_dots->pixel_data[i] > dots->pixel_data[i])
			output->pixel_data[i] = 0;
		else
			output->pixel_data[i] = dots->pixel_data[i] - no_dots->pixel_data[i];
	}
	return output;
}

void save_image(struct image* img, char* filename)
{
	int i;
	FILE *f = fopen(filename, "w");
	if (f == NULL)
	{
		printf("Error opening file!\n");
		exit(1);
	}

	fprintf(f, "/* GIMP RGB C-Source image dump (dots_img.c) */ \n");
	fprintf(f, "#include \"images.h\"\n\n");
	fprintf(f, "struct image dots_image = {\n");
	fprintf(f, "  640, 480, 3,\n");
	fprintf(f, "  {\n  ");
	for(i=0; i < (IMAGE_SIZE-1); i++)
	{
		if(i != 0 && i%WIDTH == 0)
			fprintf(f, "\n  ");
		fprintf(f,"%3d,",img->pixel_data[i]);
	}
	fprintf(f, "%3d\n  },\n};\n",img->pixel_data[i]);

	fclose(f);
}

void save_green(struct image* img, char*filename)
{
	int i;
	FILE *f = fopen(filename, "w");
	if (f == NULL)
	{
		printf("Error opening file!\n");
		exit(1);
	}

	fprintf(f, "/* green values only, 640 x 480 pix */ \n\n");
	for(i=1; i < (IMAGE_SIZE); i+=3)
	{
		if(i != 0 && (i-1)%(WIDTH*3) == 0)
			fprintf(f, "\n");
		fprintf(f,"%3d,",img->pixel_data[i]);
	}

	fclose(f);
}

/* precondition: arr must be large enough or overflow will occur */
void green_filter(struct image* img, unsigned char *arr)
{
	int i;
	for(i=1; i < (IMAGE_SIZE); i+=3)
	{
		*(arr++) = img->pixel_data[i];
	}
}

void subtraction_filter_green(unsigned char *out, unsigned char *left, unsigned char *right)
{
	for(int i=0; i < NUM_PIXELS; i++)
	{
		if(*right > *left) {
			*out = 0;
		}
		else {
			*out = *left - *right;
		}
		right++;
		left++;
		out++;
	}
}

void save_green_arr(unsigned char *arr, char* filename)
{
	int i;
	FILE *f = fopen(filename, "w");
	if (f == NULL)
	{
		printf("Error opening file!\n");
		exit(1);
	}

	fprintf(f, "/* green values only, 640 x 480 pix */ \n\n");
	for(i=1; i < (NUM_PIXELS); i++)
	{
		if(i != 0 && (i-1)%(WIDTH) == 0)
			fprintf(f, "\n");
		fprintf(f,"%3d,",*(arr++));
	}

	fclose(f);
}

int main()
{
	unsigned char working_green[NUM_PIXELS];
	unsigned char dots_green[NUM_PIXELS];
	unsigned char no_dots_green[NUM_PIXELS];

	// point finder variables
	int left, right;
	const int threshold_C = 10;
	Point points[100];
	const int unused_C = -1;
	const int active_C = 0;
	const int consemated_C = 1;
	const int bad_C = -2;
	int point_status[100];
	int left_point = 0;
	int right_point = -1;
	int center_points[100][2];


	green_filter(&dots_image, dots_green);
	green_filter(&no_dots_image, no_dots_green);
	subtraction_filter_green(working_green, dots_green, no_dots_green);

	// save_green_arr(working_green, (char*)"working.txt");
	save_green_arr(dots_green, (char*)"dots.txt");
	save_green_arr(no_dots_green, (char*)"no_dots.txt");

	// point finder algorithm
	left = right = -1;
	for(int i=0; i<100; i++)
		point_status[i] = unused_C;
	for(int i=0; i<NUM_PIXELS; i++)
	{
		if(working_green[i] > threshold_C)
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
		}
		center_points[i][0] = *center;
		center_points[i][1] = *(center+1);
		printf("%d(%d): (%d,%d)\n", i, point_status[i], *center, *(center+1));
	}

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
		for(int j = 0; j < 100 && point_status[j] != unused_C; j++)
		{
			if(point_status[j] == bad_C)
				continue;
			if(XVAL(i) == center_points[j][0] && YVAL(i) == center_points[j][1])
			{
				fprintf(f, "XXX,");
				center = true;
				break;
			}
		}
		if(!center)
			fprintf(f,"%3d,",working_green[i]);
		center = false;
	}

	fclose(f);

	return 0;
}