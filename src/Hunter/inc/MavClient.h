#ifndef GREENHUNTER_HUNTER_MAV_CLIENT_H
#define GREENHUNTER_HUNTER_MAV_CLIENT_H

#include <functional>
#include <boost/asio.hpp>

#include "Telemetry.h"
#include "Circle.h"

class MavClient
{
public:
    typedef CircleOnMap::LatLon LatLon;

    struct Params
    {
        std::string server = "localhost";
        std::string port = "6867";
        int shot_time_ms = 5000;
    };

    typedef std::function<void (void)>
        ArrivedCallback;

    typedef std::function<void (void)>
        ShotCallback;

    typedef std::function<void (const Telemetry&)>
        PositionCallback;

    MavClient(boost::asio::io_context &context,
              ArrivedCallback on_arrived,
              ShotCallback on_shot,
              PositionCallback on_position,
              const Params &params);

    void sendGoTo(const LatLon &pos);

    void sendShoot(const CircleColor &color);

private:
    void connectAsync();

    void connectRetryAsync();

    void getPosAsync();

    void readAsync();

    void handleRead(const boost::system::error_code &err,
                    size_t count);

    void handleLine(const std::string &line);

    Params _params;

    ArrivedCallback _on_arrived;
    ShotCallback _on_shot;
    PositionCallback _on_position;

    boost::asio::steady_timer _get_pos_timer;
    boost::asio::steady_timer _shot_timeout;
    boost::asio::steady_timer _retry_timeout;
    boost::asio::ip::tcp::resolver _resolver;
    boost::asio::ip::tcp::socket _socket;
    boost::asio::streambuf _buf;
};

#endif
