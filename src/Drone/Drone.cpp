#include "Drone.h"

#include <opencv2/imgcodecs.hpp>

using namespace cv;

FrameTelemetry Drone::getFrameWithTelemetry()
{
    Mat frame = imread("data/im7.jpeg");
    Telemetry telemetry = {
        0,
        0,
        0,
        10,
        0,
        0
    };

    return {
        frame,
        telemetry
    };
}
