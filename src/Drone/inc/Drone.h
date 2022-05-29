#ifndef GREENHUNTER_DRONE_H
#define GREENHUNTER_DRONE_H

#include "FrameTelemetry.h"

class Drone
{
public:
    struct CameraParams
    {
        double fov_h;
        double fov_v;

        int frame_width;
        int frame_height;
    };

    FrameTelemetry getFrameWithTelemetry();

    CameraParams getCameraParams();

private:

};

#endif
