#ifndef GREENHUNTER_TELEMETRY_H
#define GREENHUNTER_TELEMETRY_H

#include <boost/geometry.hpp>

struct Telemetry
{
    typedef boost::geometry::model::point<
        double, 2,
        boost::geometry::cs::geographic<
            boost::geometry::radian>>
        Position;

    double altitude;
    double azimuth;
    Position position;
};

#endif
