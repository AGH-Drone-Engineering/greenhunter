#include "MapLocalizer.h"

#include <cmath>
#include <boost/geometry.hpp>

namespace bg = boost::geometry;
typedef bg::model::point<double, 2, bg::cs::geographic<bg::radian>> GeopointRad;
typedef bg::model::point<double, 2, bg::cs::geographic<bg::degree>> GeopointDeg;
typedef bg::formula::vincenty_direct<double, true> vincenty;

MapLocalizer::Coords MapLocalizer::localize(const cv::Point &frame_point,
                                            const Telemetry &telemetry,
                                            const CameraParams &camera_params)
{
    double ground_width = 2 * telemetry.altitude * tan(camera_params.fov_h * (0.5 * M_PI / 180.));
    double ground_height = 2 * telemetry.altitude * tan(camera_params.fov_v * (0.5 * M_PI / 180.));

    double dx = (frame_point.x - 0.5 * camera_params.frame_width) * ground_width / camera_params.frame_width;
    double dy = (0.5 * camera_params.frame_height - frame_point.y) * ground_height / camera_params.frame_height;

    double dist = sqrt(dx*dx + dy*dy);
    double azi_rad = -atan2(dy, dx) + M_PI_2;

    auto res = vincenty::apply(
        telemetry.position.get<0>() * bg::math::d2r<double>(),
        telemetry.position.get<1>() * bg::math::d2r<double>(),
        dist,
        azi_rad,
        bg::srs::spheroid<double>()
    );

    return {
        .lat = res.lat2 * bg::math::r2d<double>(),
        .lng = res.lon2 * bg::math::r2d<double>()
    };
}
