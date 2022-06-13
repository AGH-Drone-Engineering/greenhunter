#ifndef GREENHUNTER_DRONE_TELEMETRY_SERVER_H
#define GREENHUNTER_DRONE_TELEMETRY_SERVER_H

#include <boost/enable_shared_from_this.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/asio.hpp>

class TelemetryServer
{
public:;
    class Connection : public boost::enable_shared_from_this<Connection>
    {
    public:
        typedef boost::shared_ptr<Connection> pointer;

        static pointer create(boost::asio::io_context &io_context);

        boost::asio::ip::tcp::socket& socket();

        void startRead();

    private:
        explicit Connection(boost::asio::io_context &io_context);

        void handleRead(const boost::system::error_code &err);

        boost::asio::ip::tcp::socket _socket;
        boost::asio::streambuf _buf;
    };

    TelemetryServer(boost::asio::io_context &io_context, short port);

private:
    void startAccept();

    void handleAccept(Connection::pointer conn,
                      const boost::system::error_code &err);

    boost::asio::io_context &_io_context;
    boost::asio::ip::tcp::acceptor _acceptor;
};

#endif
