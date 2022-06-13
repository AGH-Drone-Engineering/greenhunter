#ifndef GREENHUNTER_CIRCLE_MAP_H
#define GREENHUNTER_CIRCLE_MAP_H

#include <vector>

#include <opencv2/core.hpp>

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

    void draw(cv::InputOutputArray canvas);

private:
    std::vector<Circle> _circles;
};

#endif
