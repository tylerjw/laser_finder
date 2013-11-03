#ifndef DOT_H
#define DOT_H

#define WIDTH	640
#define HEIGHT	480

class Coordinate {
public:
	int x;
	int y;
	Coordinate();
	Coordinate(long index);
	void set(long index); // calculates the x,y values from the coordinates
};

// class Dot {
// private:
// 	int min[2];
// 	int max[2];
// 	int size
// 	Coordinate center;
// public:
// 	Dot();
// 	Coordinate* get_center();
// };

#endif //DOT_H