#include "Drone.h"

#include <opencv2/imgcodecs.hpp>

using namespace cv;

Drone::Drone(boost::asio::io_context &io_context,
             const std::string &camera,
             const Params &params)
    : _telem_server(io_context, params.telem_port)
    , _cap(camera)
{
    _cap.set(cv::CAP_PROP_FRAME_WIDTH, params.camera.frame_width);
    _cap.set(cv::CAP_PROP_FRAME_HEIGHT, params.camera.frame_height);
}

Drone::Drone(boost::asio::io_context &io_context,
             int camera,
             const Params &params)
    : _telem_server(io_context, params.telem_port)
    , _cap(camera)
{
    _cap.set(cv::CAP_PROP_FRAME_WIDTH, params.camera.frame_width);
    _cap.set(cv::CAP_PROP_FRAME_HEIGHT, params.camera.frame_height);
}

cv::Mat Drone::getFrame()
{
    Mat img;
    _cap.read(img);
    return img;
}

boost::optional<Telemetry> Drone::getTelemetry()
{
    return _telem_server.latest();
}

boost::optional<FrameTelemetry> Drone::getFrameWithTelemetry()
{
    auto telemetry_opt = getTelemetry();
    if (!telemetry_opt) return boost::none;
    auto telemetry = *telemetry_opt;

    auto frame = getFrame();

    return boost::make_optional<FrameTelemetry>({
        frame,
        telemetry
    });
}

void Drone::waitTelemetryValid()
{
    _telem_server.waitValid();
}
