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
    , _retry_timeout(context)
    , _resolver(context)
    , _socket(context)
{
    connectAsync();
}

void MapClient::connectRetryAsync()
{
    _retry_timeout.expires_from_now(
        ba::chrono::seconds(3)
    );

    _retry_timeout.async_wait(
        [this]
        (const error_code &err)
        {
            connectAsync();
        }
    );
}

void MapClient::connectAsync()
{
    cout << "[MapClient] Connecting" << endl;

    _resolver.async_resolve(
        _params.server,
        _params.port,
        [this]
        (const error_code &err,
         const tcp::resolver::results_type &results)
        {
            if (err)
            {
                cerr << "[MapClient] Resolution error: "
                     << err.message()
                     << endl;
                connectRetryAsync();
                return;
            }

            ba::async_connect(
                _socket,
                results,
                [this]
                (const error_code &err,
                 const tcp::endpoint &endpoint)
                {
                    if (err)
                    {
                        cerr << "[MapClient] Connection error: "
                             << err.message()
                             << endl;
                        connectRetryAsync();
                        return;
                    }

                    cout << "[MapClient] Connected" << endl;

                    _circles.clear();

                    readAsync();
                }
            );
        }
    );
}

void MapClient::readAsync()
{
    ba::async_read_until(
        _socket,
        _buf,
        '\n',
        [this]
        (const error_code &err,
         size_t count)
        {
            if (err)
            {
                cerr << "[MapClient] Read error: "
                     << err.message()
                     << endl;
                _socket.close();
                connectRetryAsync();
                return;
            }

            bool got_end = false;

            try
            {
                std::istream is(&_buf);
                std::string line;
                std::getline(is, line, '\n');

                if (line.empty())
                {
                    got_end = true;
                    _update_callback(_circles);
                }
                else
                {
                    handleLine(line);
                }
            }
            catch (const std::exception &ex)
            {
                cerr << "[MapClient] Parsing error: "
                     << ex.what()
                     << endl;
            }

            if (got_end)
                _circles.clear();

            readAsync();
        }
    );
}

void MapClient::handleLine(const std::string &line)
{
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
        cerr << "[MapClient] Invalid circle color: " << color << endl;
    }
}
