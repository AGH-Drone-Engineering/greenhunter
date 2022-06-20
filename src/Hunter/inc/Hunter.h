#ifndef GREENHUNTER_HUNTER_H
#define GREENHUNTER_HUNTER_H

#include <boost/asio.hpp>

#include "Telemetry.h"
#include "MapClient.h"
#include "MavClient.h"

class Hunter
{
public:
    typedef CircleOnMap::LatLon LatLon;

    struct Params
    {
        MapClient::Params map;
        MavClient::Params mav;
    };

    Hunter(boost::asio::io_context &context,
           const Params &params);

private:

    void onMapUpdate(const std::vector<CircleOnMap> &circles);

    void onArrived();

    void onShot();

    void onPosition(const Telemetry &telemetry);

    MapClient _map;
    MavClient _mav;
    Telemetry _telemetry;
};

#endif
