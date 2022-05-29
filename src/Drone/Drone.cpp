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
        50.093059,
        19.818654
    };

    return {
        frame,
        telemetry
    };
}

Drone::CameraParams Drone::getCameraParams()
{
    Mat frame = imread("data/im7.jpeg");
    return {
        60,
        60. * frame.size().height / frame.size().width,
        frame.size().width,
        frame.size().height
    };
}
