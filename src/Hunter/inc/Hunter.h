#ifndef GREENHUNTER_HUNTER_H
#define GREENHUNTER_HUNTER_H

#include <boost/asio.hpp>

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

    void onPosition(const LatLon &pos);

    MapClient _map;
    MavClient _mav;
};

#endif
