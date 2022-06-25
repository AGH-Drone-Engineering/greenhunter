#ifndef GREENHUNTER_DRONE_TELEMETRY_SERVER_H
#define GREENHUNTER_DRONE_TELEMETRY_SERVER_H

#include <boost/enable_shared_from_this.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/asio.hpp>
#include <boost/thread.hpp>
#include <boost/optional.hpp>

#include "Telemetry.h"

class TelemetryServer
{
public:;
    class Connection : public boost::enable_shared_from_this<Connection>
    {
    public:
        typedef boost::shared_ptr<Connection> pointer;

        static pointer create(boost::asio::io_context &io_context,
                              TelemetryServer &server);

        boost::asio::ip::tcp::socket& socket();

        void startRead();

    private:
        Connection(boost::asio::io_context &io_context,
                   TelemetryServer &server);

        void handleRead(const boost::system::error_code &err);

        void handleData();

        TelemetryServer &_server;
        boost::asio::ip::tcp::socket _socket;
        boost::asio::streambuf _buf;
    };

    TelemetryServer(boost::asio::io_context &io_context, short port);

    boost::optional<Telemetry> latest();

    void waitValid();

    void waitInAir(double altitude);

private:
    bool isValid();

    void startAccept();

    void handleAccept(Connection::pointer conn,
                      const boost::system::error_code &err);

    boost::asio::io_context &_io_context;
    boost::asio::ip::tcp::acceptor _acceptor;
    Telemetry _telemetry;
    bool _lon_valid;
    bool _lat_valid;
    bool _alt_valid;
    bool _azi_valid;
    boost::mutex _mtx;
    boost::condition_variable _valid_cond;
};

#endif
