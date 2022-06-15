#ifndef GREENHUNTER_MAP_LOCALIZER_H
#define GREENHUNTER_MAP_LOCALIZER_H

#include <opencv2/core.hpp>
#include <boost/geometry.hpp>

#include "Circle.h"
#include "Telemetry.h"
#include "CameraParams.h"

class MapLocalizer
{
public:
    CircleOnMap localize(const CircleOnFrame &circle,
                         const Telemetry &telemetry,
                         const CameraParams &camera_params);

private:

};

#endif
