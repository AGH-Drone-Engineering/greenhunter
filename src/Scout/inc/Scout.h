#ifndef GREENHUNTER_SCOUT_H
#define GREENHUNTER_SCOUT_H

#include "Drone.h"
#include "CircleDetector.h"
#include "MapLocalizer.h"
#include "CircleMap.h"

class Scout
{
public:
    void run();

private:
    Drone _drone;
    CircleDetector _detector;
    MapLocalizer _localizer;
    CircleMap _map;
};

#endif
