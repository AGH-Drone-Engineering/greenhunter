#include "Drone.h"

#include <opencv2/imgcodecs.hpp>

using namespace cv;

Drone::Drone(boost::asio::io_context &io_context, const Params &params)
    : _telem_server(io_context, params.telem_port)
{}

FrameTelemetry Drone::getFrameWithTelemetry()
{
    Mat frame = imread("data/im7.jpeg");
    frame = frame(Rect(0, 0, frame.size().height, frame.size().height));

    Telemetry telemetry = {
        0,
        0,
        0,
        10,
        50.093059,
        19.818654
    };

    CameraParams camera = {
        60,
        60. * frame.size().height / frame.size().width,
        frame.size().width,
        frame.size().height
    };

    return {
        frame,
        camera,
        telemetry
    };
}
