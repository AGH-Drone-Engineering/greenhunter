#ifndef GREENHUNTER_CIRCLE_H
#define GREENHUNTER_CIRCLE_H

#include <opencv2/core.hpp>
#include <boost/geometry.hpp>

enum class CircleColor
{
    Brown,
    Gold,
    Beige,
    WhiteSquare,
};

struct CircleOnFrame
{
    CircleColor color;
    cv::RotatedRect ellipse;
};

struct CircleOnMap
{
    typedef boost::geometry::model::point<
        double, 2, boost::geometry::cs::geographic<
            boost::geometry::radian>>
        LatLon;

    CircleColor color;
    LatLon position;
};

#endif
