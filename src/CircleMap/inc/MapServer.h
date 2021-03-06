#ifndef GREENHUNTER_DRONE_MAP_SERVER_H
#define GREENHUNTER_DRONE_MAP_SERVER_H

#include <boost/enable_shared_from_this.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/asio.hpp>

#include "CircleMap.h"

class MapServer
{
public:
    class Connection : public boost::enable_shared_from_this<Connection>
    {
    public:
        typedef boost::shared_ptr<Connection> pointer;

        static pointer create(CircleMap &map,
                              boost::asio::io_context &io_context);

        boost::asio::ip::tcp::socket& socket();

        void start();

    private:
        Connection(CircleMap &map,
                   boost::asio::io_context &io_context);

        void writeAsync();

        boost::asio::ip::tcp::socket _socket;
        boost::asio::steady_timer _timer;
        CircleMap &_map;
    };

    MapServer(boost::asio::io_context &io_context,
              CircleMap &map,
              short port);

private:
    void acceptAsync();

    boost::asio::io_context &_io_context;
    boost::asio::ip::tcp::acceptor _acceptor;
    CircleMap &_map;
};

#endif
