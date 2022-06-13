#include "CircleMap.h"

#include <iostream>
#include <opencv2/imgproc.hpp>
#include <boost/geometry.hpp>

namespace bg = boost::geometry;
typedef bg::model::point<double, 2, bg::cs::geographic<bg::radian>> GeopointRad;
typedef bg::formula::vincenty_inverse<double, true, true> vin_inv;
typedef bg::formula::vincenty_direct<double, true> vin_dir;

using namespace std;
using namespace cv;

void CircleMap::push(const Circle &circle)
{
    _circles.push_back(circle);
}

const std::vector<CircleMap::Circle>& CircleMap::getAll()
{
    return _circles;
}

void CircleMap::draw(InputOutputArray canvas)
{
    if (_circles.empty()) return;

    double lon_min = _circles[0].coords.lng;
    double lat_min = _circles[0].coords.lat;
    double lon_max = lon_min;
    double lat_max = lat_min;

    for (const auto &c : _circles)
    {
        lon_min = min(lon_min, c.coords.lng);
        lon_max = max(lon_max, c.coords.lng);
        lat_min = min(lat_min, c.coords.lat);
        lat_max = max(lat_max, c.coords.lat);
    }

    lon_min *= bg::math::d2r<double>();
    lon_max *= bg::math::d2r<double>();
    lat_min *= bg::math::d2r<double>();
    lat_max *= bg::math::d2r<double>();

    auto bottom_left_offset = vin_dir::apply(
        lon_min,
        lat_min,
        5,
        5 * M_PI_4,
        bg::srs::spheroid<double>()
    );

    auto top_right_offset = vin_dir::apply(
        lon_max,
        lat_max,
        5,
        M_PI_4,
        bg::srs::spheroid<double>()
    );

    lon_min = bottom_left_offset.lon2;
    lat_min = bottom_left_offset.lat2;

    lon_max = top_right_offset.lon2;
    lat_max = top_right_offset.lat2;

    double lon_center = 0.5 * (lon_min + lon_max);
    double lat_center = 0.5 * (lat_min + lat_max);

    double px_per_m_x = canvas.size().width /
        bg::distance(
            GeopointRad(lon_min, lat_center),
            GeopointRad(lon_max, lat_center)
        );

    double px_per_m_y = canvas.size().height /
        bg::distance(
            GeopointRad(lon_center, lat_min),
            GeopointRad(lon_center, lat_max)
        );

    double px_per_m = min(px_per_m_x, px_per_m_y);

    for (const auto &c : _circles)
    {
        auto res = vin_inv::apply(
            lon_min,
            lat_min,
            c.coords.lng * bg::math::d2r<double>(),
            c.coords.lat * bg::math::d2r<double>(),
            bg::srs::spheroid<double>()
        );

        double ang = -res.azimuth + M_PI_2;

        double xm = res.distance * cos(ang);
        double ym = res.distance * sin(ang);

        Point px(
            xm * px_per_m,
            canvas.size().height - ym * px_per_m
        );

        circle(
            canvas,
            px,
            0.5 * px_per_m,
            Scalar::all(255),
            FILLED
        );
    }
}
