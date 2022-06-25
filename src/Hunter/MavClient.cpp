#include "MavClient.h"

#include <iostream>
#include <string>

namespace b = boost;
namespace ba = b::asio;

using ba::ip::tcp;
using b::system::error_code;
using namespace std::placeholders;
using std::cout;
using std::cerr;
using std::endl;

MavClient::MavClient(ba::io_context &context,
                     ArrivedCallback on_arrived,
                     ShotCallback on_shot,
                     PositionCallback on_position,
                     const Params &params)
    : _params(params)
    , _on_arrived(std::move(on_arrived))
    , _on_shot(std::move(on_shot))
    , _on_position(std::move(on_position))
    , _get_pos_timer(context)
    , _shot_timeout(context)
    , _retry_timeout(context)
    , _resolver(context)
    , _socket(context)
{
    connectAsync();
    getPosAsync();
}

void MavClient::connectRetryAsync()
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

void MavClient::connectAsync()
{
    cout << "[MavClient] Connecting" << endl;

    _resolver.async_resolve(
        _params.server,
        _params.port,
        [this]
        (const error_code &err,
         const tcp::resolver::results_type &results)
        {
            if (err)
            {
                cerr << "[MavClient] Resolution error: "
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
                        cerr << "[MavClient] Connection error: "
                             << err.message()
                             << endl;

                        connectRetryAsync();

                        return;
                    }

                    cout << "[MavClient] Connected"
                         << endl;

                    readAsync();
                }
            );
        }
    );
}

void MavClient::readAsync()
{
    ba::async_read_until(
        _socket,
        _buf,
        '\n',
        std::bind(&MavClient::handleRead, this, _1, _2)
    );
}

void
MavClient::handleRead(const error_code &err,
                      size_t count)
{
    if (err)
    {
        cerr << "[MavClient] Read error: "
             << err.message()
             << endl;

        _socket.close();

        connectRetryAsync();

        return;
    }

    try
    {
        std::istream is(&_buf);
        std::string line;
        std::getline(is, line, '\n');
        handleLine(line);
    }
    catch (const std::exception &ex)
    {
        cerr << "[MavClient] Parsing error: "
             << ex.what()
             << endl;
    }

    readAsync();
}

void MavClient::handleLine(const std::string &line)
{
    if (line == "ACK")
    {
        _on_arrived();
    }
    else if (line.rfind("POS ", 0) == 0)
    {
        size_t i;

        auto lat_str = line.substr(4);
        double lat = std::stod(lat_str, &i);

        auto lon_str = lat_str.substr(i + 1);
        double lon = std::stod(lon_str, &i);

        auto azi_str = lon_str.substr(i + 1);
        double azi = std::stod(azi_str, &i);

        auto alt_str = azi_str.substr(i + 1);
        double alt = std::stod(alt_str, &i);

        _on_position({
            alt,
            azi * b::geometry::math::d2r<double>(),
            {
                lon * b::geometry::math::d2r<double>(),
                lat * b::geometry::math::d2r<double>(),
            },
        });
    }
    else
    {
        cerr << "[MavClient] Unknown packet '"
             << line
             << "'"
             << endl;
    }
}

void MavClient::sendGoTo(const LatLon &pos)
{
    std::stringstream msg;
    msg.precision(15);

    msg << "GOTO "
        << pos.get<1>() * b::geometry::math::r2d<double>()
        << ","
        << pos.get<0>() * b::geometry::math::r2d<double>()
        << "\n";

    msg.flush();

    ba::async_write(
        _socket,
        ba::buffer(msg.str()),
        []
        (const error_code &err, size_t count)
        {
            if (err)
            {
                cerr << "[MavClient] Could not send GOTO: "
                     << err.message()
                     << endl;
            }
        }
    );
}

void MavClient::sendShoot(const CircleColor &color)
{
    std::stringstream msg;

    msg << "SHOOT "
        << static_cast<int>(color)
        << "\n";

    msg.flush();

    ba::async_write(
        _socket,
        ba::buffer(msg.str()),
        []
        (const error_code &err, size_t count)
        {
            if (err)
            {
                cerr << "[MavClient] Could not send SHOOT: "
                     << err.message()
                     << endl;
            }
        }
    );

    // ignore potential errors to fix lockups

    _shot_timeout.expires_after(
        ba::chrono::milliseconds(_params.shot_time_ms)
    );

    _shot_timeout.async_wait(
        [this]
        (const error_code &err)
        {
            _on_shot();
        }
    );
}

void MavClient::getPosAsync()
{
    std::string msg = "GETPOS\n";
    ba::async_write(
        _socket,
        ba::buffer(msg),
        [this]
        (const error_code &err, size_t count)
        {
            if (err)
            {
                cerr << "[MavClient] Could not send GETPOS: "
                     << err.message()
                     << endl;
            }

            _get_pos_timer.expires_after(
                ba::chrono::seconds(1)
            );

            _get_pos_timer.async_wait(
                [this]
                (const error_code &err)
                {
                    getPosAsync();
                }
            );
        }
    );
}
