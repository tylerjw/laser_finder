#ifndef POINT_H
#define POINT_H

#define DEBUG_FILE

#define WIDTH	640
#define HEIGHT	480
#define NUM_PIXELS	(WIDTH*HEIGHT)

#define XVAL(idx)	(idx%WIDTH)
#define YVAL(idx)	(idx/WIDTH)

#define THRESHOLD	10	// threshold for point test
#define MIN_SIZE	3
#define MAX_SIZE	(30*30)
#define MIN_SKEW	-5
#define MAX_SKEW	5

class Coordinate {
public:
	int x;
	int y;
	Coordinate();
	Coordinate(long index);
	void set(long index); // calculates the x,y values from the coordinates
};

class Point {
private:
	int min[2];
	int max[2];
	int size;
	int center[2];
public:
	Point();
	void add_line(int left, int right); // adds a line of pixels to POINT
	void add_point(int index);
	int* get_center(); // calculates center and returns pointer to center
	int test_point(int index); // x-failure: 0, y-falure: -1, new point and success = 1
	int test_shape(); // 
};

/** point finder
takes the working green array, finds center points

returns number of center points
*/
int point_finder(unsigned char* working_green, int (*center_points)[2], int length);

#endif //POINT_H