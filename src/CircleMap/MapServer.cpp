#include "MapServer.h"

#include <iostream>

namespace b = boost;
namespace ba = b::asio;

using b::system::error_code;
using ba::ip::tcp;
using std::cout;
using std::cerr;
using std::endl;

MapServer::MapServer(ba::io_context &io_context,
                     CircleMap &map,
                     short port)
    : _io_context(io_context)
    , _acceptor(io_context, tcp::endpoint(tcp::v4(), port))
    , _map(map)
{
    acceptAsync();
}

void MapServer::acceptAsync()
{
    auto conn = Connection::create(_map, _io_context);
    _acceptor.async_accept(
        conn->socket(),
        [this, conn]
        (const error_code &err)
        {
            if (err)
            {
                cerr << "[MapServer] Accept error: "
                     << err.message()
                     << endl;
                acceptAsync();
                return;
            }

            cout << "[MapServer] New connection" << endl;
            conn->start();

            acceptAsync();
        }
    );
}

MapServer::Connection::Connection(CircleMap &map,
                                  ba::io_context &io_context)
        : _timer(io_context)
        , _socket(io_context)
        , _map(map)
{}

MapServer::Connection::pointer
MapServer::Connection::create(CircleMap &map,
                              ba::io_context &io_context)
{
    return pointer(new Connection(map, io_context));
}

tcp::socket& MapServer::Connection::socket()
{
    return _socket;
}

void MapServer::Connection::start()
{
    _timer.expires_at(ba::chrono::steady_clock::now());

    writeAsync();
}

void MapServer::Connection::writeAsync()
{
    std::stringstream msg;
    msg.precision(15);
    for (const auto &c : _map.getAll())
    {
        msg << c.position.get<0>() << ","
            << c.position.get<1>() << ","
            << (c.color == CircleColor::Brown
                    ? "BROWN" :
                c.color == CircleColor::Gold
                    ? "GOLD" :
                c.color == CircleColor::Beige
                    ? "BEIGE"
                    : "WHITESQUARE")
            << "\n";
    }
    msg << "\n";
    msg.flush();

    pointer conn = shared_from_this();

    boost::asio::async_write(
        _socket,
        boost::asio::buffer(msg.str()),
        [conn]
        (const error_code &err, size_t count)
        {
            if (err)
            {
                cerr << "[MapServer] Write error: "
                     << err.message()
                     << endl;
                conn->socket().close();
                return;
            }

            conn->_timer.expires_at(
                conn->_timer.expiry() + ba::chrono::seconds(1)
            );

            conn->_timer.async_wait(
                [conn]
                (const error_code &err)
                {
                    conn->writeAsync();
                }
            );
        }
    );
}
