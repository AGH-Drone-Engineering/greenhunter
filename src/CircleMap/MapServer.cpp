#include "MapServer.h"

#include <iostream>
#include <boost/bind/bind.hpp>

using boost::asio::ip::tcp;
using boost::asio::io_context;
using std::cout;
using std::cerr;
using std::endl;

MapServer::MapServer(io_context &io_context,
                     CircleMap &map,
                     short port)
    : _io_context(io_context)
    , _acceptor(io_context, tcp::endpoint(tcp::v4(), port))
    , _map(map)
{
    startAccept();
}

void MapServer::startAccept()
{
    auto conn = Connection::create(_map, _io_context);
    _acceptor.async_accept(
        conn->socket(),
        boost::bind(
            &MapServer::handleAccept,
            this,
            conn,
            boost::asio::placeholders::error
        )
    );
}

void MapServer::handleAccept(Connection::pointer conn,
                             const boost::system::error_code &err)
{
    if (err)
    {
        cerr << "Connection error" << endl;
    }
    else
    {
        cout << "New connection" << endl;
        conn->startWrite();
    }
    startAccept();
}

MapServer::Connection::Connection(CircleMap &map,
                                  io_context &io_context)
        : _socket(io_context)
        , _map(map)
{}

MapServer::Connection::pointer
MapServer::Connection::create(CircleMap &map,
                              io_context &io_context)
{
    return pointer(new Connection(map, io_context));
}

tcp::socket& MapServer::Connection::socket()
{
    return _socket;
}

void MapServer::Connection::startWrite()
{
    std::stringstream msg;
    msg.precision(15);
    for (const auto &c : _map.getAll())
    {
        msg << c.coords.get<0>() << "," << c.coords.get<1>() << "\n";
    }
    msg.flush();
    boost::asio::async_write(
        _socket,
        boost::asio::buffer(msg.str()),
        boost::bind(
            &Connection::handleWrite,
            shared_from_this(),
            boost::asio::placeholders::error
        )
    );
}

void MapServer::Connection::handleWrite(const boost::system::error_code &err)
{
    if (err)
        cerr << "Write error" << endl;
    else
        cout << "Write ok" << endl;

    _socket.close();
}
