// laser_finder.c - finds the location of laser points comparing two images
#include "images.h"
#include <stdio.h>
#include <stdlib.h>

extern struct image no_dots_image;
extern struct image dots_image;

/*
	output = dots - no_dots
*/
struct image* subtraction_filter(struct image* output, struct image* dots, struct image* no_dots)
{
	int i;
	for(i=0; i < (640*480*3); i++)
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
	for(i=0; i < (640*480*3-1); i++)
	{
		if(i != 0 && i%640 == 0)
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
	for(i=1; i < (640*480*3); i+=3)
	{
		if(i != 0 && (i-1)%(640*3) == 0)
			fprintf(f, "\n");
		fprintf(f,"%3d,",img->pixel_data[i]);
	}

	fclose(f);
}

int main()
{
	struct image working_image;

	subtraction_filter(&working_image, &dots_image, &no_dots_image);

	save_green(&working_image, "working.txt");
	save_green(&dots_image, "dots.txt");
	save_green(&no_dots_image, "no_dots.txt");

	return 0;
}