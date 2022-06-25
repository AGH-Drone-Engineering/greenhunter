#include "TelemetryServer.h"

#include <iostream>
#include <boost/bind/bind.hpp>
#include <boost/algorithm/string.hpp>
#include <boost/geometry.hpp>

using boost::asio::ip::tcp;
using boost::asio::io_context;

using std::cout;
using std::cerr;
using std::endl;

TelemetryServer::TelemetryServer(io_context &io_context, short port)
    : _io_context(io_context)
    , _acceptor(io_context, tcp::endpoint(tcp::v4(), port))
    , _telemetry({})
    , _lon_valid(false)
    , _lat_valid(false)
    , _alt_valid(false)
    , _azi_valid(false)
{
    startAccept();
}

boost::optional<Telemetry> TelemetryServer::latest()
{
    boost::lock_guard guard(_mtx);
    return boost::make_optional(isValid(), _telemetry);
}

void TelemetryServer::waitValid()
{
    boost::unique_lock lock(_mtx);
    while (!isValid()) _valid_cond.wait(lock);
}

void TelemetryServer::waitInAir(double altitude)
{
    boost::unique_lock lock(_mtx);
    while (!isValid() || _telemetry.altitude < altitude)
        _valid_cond.wait(lock);
}

bool TelemetryServer::isValid()
{
    return _lon_valid && _lat_valid && _alt_valid && _azi_valid;;
}

void TelemetryServer::startAccept()
{
    auto conn = Connection::create(_io_context, *this);
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

TelemetryServer::Connection::Connection(io_context &io_context,
                                        TelemetryServer &server)
        : _server(server)
        , _socket(io_context)
{}

TelemetryServer::Connection::pointer
TelemetryServer::Connection::create(io_context &io_context,
                                    TelemetryServer &server)
{
    return pointer(new Connection(io_context, server));
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
        try
        {
            std::istream is(&_buf);
            std::string line;
            std::getline(is, line, ',');
            handleLine(line);
        }
        catch (const std::exception &ex)
        {
            cerr << "[TelemetryServer] Parsing error: "
                 << ex.what()
                 << endl;
        }

        startRead();
    }
    else
    {
        _socket.close();
        cout << "Closed" << endl;
    }
}

void TelemetryServer::Connection::handleLine(std::string line)
{
    boost::algorithm::trim(line);

    char type = line[0];
    line.erase(0, 1);
    double val = std::stod(line);

    {
        boost::lock_guard guard(_server._mtx);
        switch (type)
        {
            case 'y':
                cout << "Got yaw " << val << endl;
                _server._telemetry.azimuth = val * boost::geometry::math::d2r<double>();
                _server._azi_valid = true;
                break;

            case 'l':
                cout << "Got latitude " << val << endl;
                _server._telemetry.position.set<1>(val * boost::geometry::math::d2r<double>());
                _server._lat_valid = true;
                break;

            case 'g':
                cout << "Got longitude " << val << endl;
                _server._telemetry.position.set<0>(val * boost::geometry::math::d2r<double>());
                _server._lon_valid = true;
                break;

            case 'a':
                cout << "Got altitude " << val << endl;
                _server._telemetry.altitude = val;
                _server._alt_valid = true;
                break;

            default:
                cout << "Got unknown type '" << type << "'" << endl;
                break;
        }
    }

    _server._valid_cond.notify_all();
}
