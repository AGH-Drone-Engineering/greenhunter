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
        int update_delay_ms = 1000;
        std::string server = "localhost";
        std::string port = "6869";
    };

    typedef std::function<void (const std::vector<CircleOnMap>&)>
        UpdateCallback;

    MapClient(boost::asio::io_context &context,
              UpdateCallback update_callback,
              const Params &params);

private:

    void queryMapAsync();

    void queryMapLaterAsync();

    void handleResolve(const boost::system::error_code &err,
                       const boost::asio::ip::tcp::resolver::results_type &results);

    void handleConnect(const boost::system::error_code &err,
                       const boost::asio::ip::tcp::endpoint &endpoint);

    void handleRead(const boost::system::error_code &err,
                    std::size_t count);

    Params _params;
    UpdateCallback _update_callback;

    boost::asio::steady_timer _timer;
    boost::asio::ip::tcp::resolver _resolver;
    boost::asio::ip::tcp::socket _socket;
    boost::asio::streambuf _buf;

    std::vector<CircleOnMap> _circles;
};

#endif
