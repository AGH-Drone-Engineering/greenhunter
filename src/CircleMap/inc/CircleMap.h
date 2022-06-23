#ifndef GREENHUNTER_CIRCLE_MAP_H
#define GREENHUNTER_CIRCLE_MAP_H

#include <vector>
#include <opencv2/core.hpp>
#include <boost/thread.hpp>

#include "MapLocalizer.h"
#include "Circle.h"

class CircleMap
{
public:
    typedef CircleOnMap::LatLon LatLon;

    struct Params
    {
        double clustering_distance = 2.;
        int min_detections = 5;
    };

    struct CircleCluster
    {
        LatLon position;
        int brown_votes;
        int gold_votes;
        int beige_votes;
    };

    explicit CircleMap(const Params &params);

    void push(const CircleOnMap &circle);

    std::vector<CircleOnMap> getAll();

    void draw(cv::InputOutputArray canvas);

private:
    Params _params;
    std::vector<CircleCluster> _clusters;
    boost::mutex _mtx;
};

#endif
