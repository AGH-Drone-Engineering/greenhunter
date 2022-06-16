#include "MapLocalizer.h"

#include <cmath>
#include <boost/geometry.hpp>

namespace bg = boost::geometry;
typedef bg::formula::vincenty_direct<double, true> vincenty;

CircleOnMap MapLocalizer::localize(const CircleOnFrame &circle,
                                   const Telemetry &telemetry,
                                   const CameraParams &camera_params) const
{
    const auto &frame_point = circle.ellipse.center;

    double ground_width = 2. * telemetry.altitude * tan(camera_params.fov_h * 0.5);
    double ground_height = 2. * telemetry.altitude * tan(camera_params.fov_v * 0.5);

    double dx = (frame_point.x - 0.5 * camera_params.frame_width) * ground_width / camera_params.frame_width;
    double dy = (0.5 * camera_params.frame_height - frame_point.y) * ground_height / camera_params.frame_height;

    double dist = sqrt(dx*dx + dy*dy);
    double azi_rad = -atan2(dy, dx) + M_PI_2 + telemetry.azimuth;

    auto res = vincenty::apply(
        telemetry.position.get<0>(),
        telemetry.position.get<1>(),
        dist,
        azi_rad,
        bg::srs::spheroid<double>()
    );

    return {
        circle.color,
        {res.lon2, res.lat2},
    };
}
