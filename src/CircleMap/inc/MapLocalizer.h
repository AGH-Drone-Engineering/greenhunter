#ifndef GREENHUNTER_MAP_LOCALIZER_H
#define GREENHUNTER_MAP_LOCALIZER_H

#include <opencv2/core.hpp>
#include <boost/geometry.hpp>

#include "Telemetry.h"
#include "CameraParams.h"

class MapLocalizer
{
public:
    typedef boost::geometry::model::point<
        double, 2, boost::geometry::cs::geographic<
            boost::geometry::radian>>
        Coords;

    Coords localize(const cv::Point &frame_point,
                    const Telemetry &telemetry,
                    const CameraParams &camera_params);

private:

};

#endif
