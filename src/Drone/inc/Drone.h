#ifndef GREENHUNTER_DRONE_H
#define GREENHUNTER_DRONE_H

#include <boost/asio.hpp>
#include <boost/optional.hpp>
#include <opencv2/videoio.hpp>

#include "CameraParams.h"
#include "FrameTelemetry.h"
#include "TelemetryServer.h"

class Drone
{
public:
    struct Params
    {
        short telem_port = 6868;
        CameraParams camera;
    };

    Drone(boost::asio::io_context &io_context,
          const std::string &camera,
          const Params &params);

    Drone(boost::asio::io_context &io_context,
          int camera,
          const Params &params);

    cv::Mat getFrame();

    boost::optional<Telemetry> getTelemetry();

    boost::optional<FrameTelemetry> getFrameWithTelemetry();

    void waitTelemetryValid();

private:
    TelemetryServer _telem_server;
    cv::VideoCapture _cap;
};

#endif
