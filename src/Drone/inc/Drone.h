#ifndef GREENHUNTER_DRONE_H
#define GREENHUNTER_DRONE_H

#include <boost/asio.hpp>

#include "FrameTelemetry.h"
#include "TelemetryServer.h"

class Drone
{
public:
    struct Params
    {
        short telem_port = 6868;
    };

    Drone(boost::asio::io_context &io_context, const Params &params);

    FrameTelemetry getFrameWithTelemetry();

private:
    TelemetryServer _telem_server;
};

#endif
