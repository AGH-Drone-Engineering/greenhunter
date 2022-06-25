#ifndef GREENHUNTER_SCOUT_H
#define GREENHUNTER_SCOUT_H

#include <boost/asio.hpp>

#include "Drone.h"
#include "CircleDetector.h"
#include "CircleSquareDetector.h"
#include "MapLocalizer.h"
#include "CircleMap.h"
#include "MapServer.h"
#include "MissionLogger.h"

#define CIRCLE_DETECTOR CircleDetector

class Scout
{
public:
    struct Params
    {
        MissionLogger::Params logger;
        Drone::Params drone;
        CircleMap::Params map;
        CIRCLE_DETECTOR::Config detector;
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
    bool mainLoop();

    Params _params;
    Drone _drone;
    CIRCLE_DETECTOR _detector;
    MapLocalizer _localizer;
    CircleMap _map;
    MapServer _map_server;
    MissionLogger _logger;
};

#endif
