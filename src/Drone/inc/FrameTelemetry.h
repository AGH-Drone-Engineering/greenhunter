#ifndef GREENHUNTER_FRAME_TELEMETRY_H
#define GREENHUNTER_FRAME_TELEMETRY_H

#include <opencv2/core.hpp>

#include "Telemetry.h"

struct FrameTelemetry
{
    cv::Mat frame;
    Telemetry telemetry;
};

#endif
