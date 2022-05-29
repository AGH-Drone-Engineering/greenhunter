#ifndef GREENHUNTER_CIRCLE_MAP_H
#define GREENHUNTER_CIRCLE_MAP_H

#include <vector>

#include "MapLocalizer.h"

class CircleMap
{
public:
    struct Circle
    {
        MapLocalizer::Coords coords;
    };

    void push(const Circle &circle);

    const std::vector<Circle>& getAll();

private:
    std::vector<Circle> _circles;
};

#endif
