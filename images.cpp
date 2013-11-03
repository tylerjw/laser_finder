#include "images.h"
#include <stdio.h>
#include <stdlib.h>

void subtraction_green_filter(unsigned char *out, struct image* left, struct image* right)
{
	int i;
	for(i=1;i<IMAGE_SIZE;i+=3)
	{
		if(right->pixel_data[i] > left->pixel_data[i])
		{
			*out = 0;
		} else {
			*(out) = left->pixel_data[i] - right->pixel_data[i];
		}
		out++;
	}
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