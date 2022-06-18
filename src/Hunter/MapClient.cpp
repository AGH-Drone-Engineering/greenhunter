#include "MapClient.h"

#include <iostream>

namespace b = boost;
namespace ba = boost::asio;

using ba::ip::tcp;
using b::system::error_code;
using std::cout;
using std::cerr;
using std::endl;

using namespace std::placeholders;

MapClient::MapClient(ba::io_context &context,
                     UpdateCallback update_callback,
                     const MapClient::Params &params)
    : _params(params)
    , _update_callback(std::move(update_callback))
    , _timer(context, ba::chrono::steady_clock::now())
    , _resolver(context)
    , _socket(context)
{
    queryMapAsync();
}

void MapClient::queryMapAsync()
{
    _resolver.async_resolve(
        _params.server,
        _params.port,
        std::bind(&MapClient::handleResolve, this, _1, _2)
    );
}

void MapClient::queryMapLaterAsync()
{
    _timer.expires_at(
        _timer.expiry() +
        ba::chrono::milliseconds(_params.update_delay_ms)
    );

    _timer.async_wait(
        [this]
        (const error_code &err)
        {
            queryMapAsync();
        }
    );
}

void
MapClient::handleResolve(const error_code &err,
                         const tcp::resolver::results_type &results)
{
    if (err)
    {
        cerr << "[MapClient] Resolution error: " << err.message() << endl;
        queryMapLaterAsync();
        return;
    }

    ba::async_connect(
        _socket,
        results,
        std::bind(&MapClient::handleConnect, this, _1, _2)
    );
}

void
MapClient::handleConnect(const error_code &err,
                         const tcp::endpoint &endpoint)
{
    if (err)
    {
        cerr << "[MapClient] Connection error: " << err.message() << endl;
        queryMapLaterAsync();
        return;
    }

    _circles.clear();

    ba::async_read_until(
        _socket,
        _buf,
        '\n',
        std::bind(&MapClient::handleRead, this, _1, _2)
    );
}

void
MapClient::handleRead(const error_code &err,
                      std::size_t count)
{
    if (err)
    {
        _update_callback(_circles);
        queryMapLaterAsync();
        return;
    }

    std::istream is(&_buf);
    std::string line;
    std::getline(is, line, '\n');
    size_t i, j;
    double lon = std::stod(line, &i);
    double lat = std::stod(line.substr(i + 1), &j);
    std::string color = line.substr(i + j + 2);
    if (color == "BROWN")
    {
        _circles.push_back({
            CircleColor::Brown,
            {lon, lat},
        });
    }
    else if (color == "GOLD")
    {
        _circles.push_back({
            CircleColor::Gold,
            {lon, lat},
        });
    }
    else if (color == "BEIGE")
    {
        _circles.push_back({
            CircleColor::Beige,
            {lon, lat},
        });
    }
    else
    {
        cerr << "Invalid circle color: " << color << endl;
    }

    ba::async_read_until(
        _socket,
        _buf,
        '\n',
        std::bind(&MapClient::handleRead, this, _1, _2)
    );
}
