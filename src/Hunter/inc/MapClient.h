#ifndef GREENHUNTER_HUNTER_MAP_CLIENT_H
#define GREENHUNTER_HUNTER_MAP_CLIENT_H

#include <functional>
#include <boost/asio.hpp>

#include "Circle.h"

class MapClient
{
public:
    struct Params
    {
        std::string server = "localhost";
        std::string port = "6869";
    };

    typedef std::function<void (const std::vector<CircleOnMap>&)>
        UpdateCallback;

    MapClient(boost::asio::io_context &context,
              UpdateCallback update_callback,
              const Params &params);

private:

    void connectAsync();

    void connectRetryAsync();

    void readAsync();

    void handleLine(const std::string &line);

    Params _params;
    UpdateCallback _update_callback;

    boost::asio::steady_timer _retry_timeout;
    boost::asio::ip::tcp::resolver _resolver;
    boost::asio::ip::tcp::socket _socket;
    boost::asio::streambuf _buf;

    std::vector<CircleOnMap> _circles;
};

#endif
