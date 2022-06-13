#include "TelemetryServer.h"

#include <iostream>
#include <boost/bind/bind.hpp>
#include <boost/algorithm/string.hpp>

using boost::asio::ip::tcp;
using boost::asio::io_context;

using std::cout;
using std::cerr;
using std::endl;

TelemetryServer::TelemetryServer(io_context &io_context, short port)
    : _io_context(io_context)
    , _acceptor(io_context, tcp::endpoint(tcp::v4(), port))
{
    startAccept();
}

void TelemetryServer::startAccept()
{
    auto conn = Connection::create(_io_context);
    _acceptor.async_accept(
        conn->socket(),
        boost::bind(
            &TelemetryServer::handleAccept,
            this,
            conn,
            boost::asio::placeholders::error
        )
    );
}

void TelemetryServer::handleAccept(Connection::pointer conn,
                                   const boost::system::error_code &err)
{
    if (err)
    {
        cerr << "Connection error" << endl;
    }
    else
    {
        cout << "New connection" << endl;
        conn->startRead();
    }
    startAccept();
}

TelemetryServer::Connection::Connection(io_context &io_context)
        : _socket(io_context)
{}

TelemetryServer::Connection::pointer TelemetryServer::Connection::create(io_context &io_context)
{
    return pointer(new Connection(io_context));
}

tcp::socket& TelemetryServer::Connection::socket()
{
    return _socket;
}

void TelemetryServer::Connection::startRead()
{
    boost::asio::async_read_until(
        _socket,
        _buf,
        ',',
        boost::bind(
            &Connection::handleRead,
            shared_from_this(),
            boost::asio::placeholders::error
        )
    );
}

void TelemetryServer::Connection::handleRead(const boost::system::error_code &err)
{
    if (!err)
    {
        std::istream is(&_buf);
        std::string line;
        std::getline(is, line, ',');
        boost::algorithm::trim(line);

        char type = line[0];
        line.erase(0, 1);
        double val = std::stod(line);

        switch (type)
        {
            case 'y':
                cout << "Got yaw " << val << endl;
                break;

            case 'l':
                cout << "Got latitude " << val << endl;
                break;

            case 'g':
                cout << "Got longitude " << val << endl;
                break;

            case 'a':
                cout << "Got altitude " << val << endl;
                break;

            default:
                cout << "Got unknown type '" << type << "'" << endl;
                break;
        }

        startRead();
    }
    else
    {
        _socket.close();
        cout << "Closed" << endl;
    }
}
