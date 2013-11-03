#include "dot.hpp"

using namespace std;

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