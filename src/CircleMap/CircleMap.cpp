#include "CircleMap.h"

#include <iostream>
#include <opencv2/imgproc.hpp>
#include <boost/geometry.hpp>

namespace bg = boost::geometry;
typedef bg::formula::vincenty_inverse<double, true, true> vin_inv;
typedef bg::formula::vincenty_direct<double, true> vin_dir;

using namespace std;
using namespace cv;

void CircleMap::push(const CircleOnMap &circle)
{
    _clusters.push_back({
        circle.position,
        0,
        0,
        0,
    });
}

std::vector<CircleOnMap> CircleMap::getAll()
{
    vector<CircleOnMap> out;
    transform(
        _clusters.cbegin(),
        _clusters.cend(),
        back_inserter(out),
        [] (const CircleCluster &c) {
            return CircleOnMap({
                CircleColor::Brown,
                c.position
            });
        }
    );
    return out;
}

void CircleMap::draw(InputOutputArray canvas)
{
    const auto circles = getAll();

    if (circles.empty()) return;

    double lon_min = circles[0].position.get<0>();
    double lat_min = circles[0].position.get<1>();
    double lon_max = lon_min;
    double lat_max = lat_min;

    for (const auto &c : circles)
    {
        lon_min = min(lon_min, c.position.get<0>());
        lon_max = max(lon_max, c.position.get<0>());
        lat_min = min(lat_min, c.position.get<1>());
        lat_max = max(lat_max, c.position.get<1>());
    }

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
            LatLon(lon_min, lat_center),
            LatLon(lon_max, lat_center)
        );

    double px_per_m_y = canvas.size().height /
        bg::distance(
            LatLon(lon_center, lat_min),
            LatLon(lon_center, lat_max)
        );

    double px_per_m = min(px_per_m_x, px_per_m_y);

    for (const auto &c : circles)
    {
        auto res = vin_inv::apply(
            lon_min,
            lat_min,
            c.position.get<0>(),
            c.position.get<1>(),
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
