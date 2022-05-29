#include "CircleMap.h"


void CircleMap::push(const Circle &circle)
{
    _circles.push_back(circle);
}

const std::vector<CircleMap::Circle>& CircleMap::getAll()
{
    return _circles;
}
