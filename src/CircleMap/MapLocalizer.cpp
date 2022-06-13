#include "MapLocalizer.h"

#include <cmath>

using namespace std;
using namespace cv;

static constexpr double R_EARTH = 6371000;

MapLocalizer::Coords MapLocalizer::localize(const Point &frame_point,
                                            const Telemetry &telemetry,
                                            const CameraParams &camera_params)
{
    double ground_width = 2 * telemetry.alt * tan(camera_params.fov_h * 0.5 * M_PI / 180);
    double ground_height = 2 * telemetry.alt * tan(camera_params.fov_v * 0.5 * M_PI / 180);

    double dx = (frame_point.x - 0.5 * camera_params.frame_width) * ground_width / camera_params.frame_width;
    double dy = (0.5 * camera_params.frame_height - frame_point.y) * ground_height / camera_params.frame_height;

    return {
        telemetry.lat + (dy / R_EARTH) * (180 / M_PI),
        telemetry.lng + (dx / R_EARTH) * (180 / M_PI) / cos(telemetry.lat * M_PI / 180)
    };
}
