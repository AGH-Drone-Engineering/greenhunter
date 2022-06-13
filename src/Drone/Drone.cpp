#include "Drone.h"

#include <opencv2/imgcodecs.hpp>
#include <boost/geometry.hpp>

using namespace cv;

Drone::Drone(boost::asio::io_context &io_context, const Params &params)
    : _telem_server(io_context, params.telem_port)
{}

cv::Mat Drone::getFrame()
{
    return imread("data/im7.jpeg");
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

    CameraParams camera = {
        60. * frame.size().aspectRatio() * boost::geometry::math::d2r<double>(),
        60. * boost::geometry::math::d2r<double>(),
        frame.size().width,
        frame.size().height
    };

    return boost::make_optional<FrameTelemetry>({
        frame,
        camera,
        telemetry
    });
}

void Drone::waitTelemetryValid()
{
    _telem_server.waitValid();
}
