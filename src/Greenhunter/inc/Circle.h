#ifndef GREENHUNTER_CIRCLE_H
#define GREENHUNTER_CIRCLE_H

#include <opencv2/core.hpp>
#include <boost/geometry.hpp>
#include <string>

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
    using boost::geometry::math::r2d;
    std::stringstream str;
    str.precision(8);
    str << circle.position.get<1>() * r2d<double>() << ","
        << circle.position.get<0>() * r2d<double>() << ","
        << circle_to_string(circle.color);
    return str.str();
}

#endif
