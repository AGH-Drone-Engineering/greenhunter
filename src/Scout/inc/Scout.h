#ifndef GREENHUNTER_SCOUT_H
#define GREENHUNTER_SCOUT_H

#include <boost/asio.hpp>

#include "Drone.h"
#include "CircleDetector.h"
#include "MapLocalizer.h"
#include "CircleMap.h"
#include "MapServer.h"

class Scout
{
public:
    struct Params
    {
        Drone::Params drone;
        CircleMap::Params map;
        CircleDetector::Config detector;
        short map_port = 6869;
        double min_altitude = 4.;
    };

    Scout(boost::asio::io_context &io_context,
          const std::string &camera,
          const Params &params);

    Scout(boost::asio::io_context &io_context,
          int camera,
          const Params &params);

    void run();

private:
    Params _params;
    Drone _drone;
    CircleDetector _detector;
    MapLocalizer _localizer;
    CircleMap _map;
    MapServer _map_server;
};

#endif
