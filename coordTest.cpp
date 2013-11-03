#include "point.h"
#include <iostream>

using namespace std;

void print_point(Coordinate* point)
{
	cout << point->x;
	cout << ", ";
	cout << point->y;
	cout << '\n';
}

int main()
{
	int x = 0;
	int y = 0;
	Coordinate point(x+(y*WIDTH));
	for(y = 475; y < HEIGHT; y++)
	{
		cout << "x, y\n";
		for(x = 635; x < WIDTH; x++)
		{
			cout << (x+(y*WIDTH)) << endl;
			point.set(x+(y*WIDTH));
			print_point(&point);
		}
	}

	return 0;
}