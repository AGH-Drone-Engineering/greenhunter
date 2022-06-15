#ifndef GREENHUNTER_CIRCLE_MAP_H
#define GREENHUNTER_CIRCLE_MAP_H

#include <vector>
#include <opencv2/core.hpp>

#include "MapLocalizer.h"
#include "Circle.h"

class CircleMap
{
public:
    typedef CircleOnMap::LatLon LatLon;

    struct CircleCluster
    {
        LatLon position;
        int brown_votes;
        int gold_votes;
        int beige_votes;
    };

    void push(const CircleOnMap &circle);

    std::vector<CircleOnMap> getAll();

    void draw(cv::InputOutputArray canvas);

private:
    std::vector<CircleCluster> _clusters;
};

#endif
