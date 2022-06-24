#ifndef GREENHUNTER_POSITION_H
#define GREENHUNTER_POSITION_H

#include <boost/geometry.hpp>

typedef boost::geometry::model::point<
    double, 2, boost::geometry::cs::geographic<
        boost::geometry::radian>>
    Position;

#endif
