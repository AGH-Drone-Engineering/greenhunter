#ifndef GREENHUNTER_MAP_LOCALIZER_H
#define GREENHUNTER_MAP_LOCALIZER_H

#include <opencv2/core.hpp>

#include "Telemetry.h"

class MapLocalizer
{
public:
    struct Coords
    {
        double lat;
        double lng;
    };

    Coords localize(const cv::Point &frame_point, const Telemetry &telemetry);

private:

};

#endif
