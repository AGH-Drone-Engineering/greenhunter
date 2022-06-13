#ifndef GREENHUNTER_SCOUT_H
#define GREENHUNTER_SCOUT_H

#include <boost/asio.hpp>

#include "Drone.h"
#include "CircleDetector.h"
#include "MapLocalizer.h"
#include "CircleMap.h"

class Scout
{
public:
    struct Params
    {
        Drone::Params drone_params;
    };

    Scout(boost::asio::io_context &io_context, const Params &params);

    void run();

private:
    Drone _drone;
    CircleDetector _detector;
    MapLocalizer _localizer;
    CircleMap _map;
};

#endif
