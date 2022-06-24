#ifndef GREENHUNTER_CIRCLE_H
#define GREENHUNTER_CIRCLE_H

#include <opencv2/core.hpp>
#include <boost/geometry.hpp>
#include <string>

#include "Position.h"

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
    typedef Position LatLon;
    CircleColor color;
    Position position;
};

static std::string position_to_string_deg(const Position &position)
{
    using boost::geometry::math::r2d;
    std::stringstream ss;
    ss.precision(8);
    ss << position.get<1>() * r2d<double>() << ","
       << position.get<0>() * r2d<double>();
    return ss.str();
}

static std::string circle_to_string(const CircleColor &color)
{
    switch (color)
    {
        case CircleColor::Brown:
            return "Brown";
        case CircleColor::Gold:
            return "Gold";
        case CircleColor::Beige:
            return "Beige";
        case CircleColor::WhiteSquare:
            return "WhiteSquare";
        default:
            return "Unknown";
    }
}

static std::string circle_to_string(const CircleOnMap &circle)
{
    return position_to_string_deg(circle.position) +
           + "," + circle_to_string(circle.color);
}

#endif
