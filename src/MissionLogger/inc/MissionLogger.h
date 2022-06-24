#ifndef GREENHUNTER_MISSION_LOGGER_H
#define GREENHUNTER_MISSION_LOGGER_H

#include <string>
#include <vector>
#include <opencv2/core.hpp>
#include <boost/thread.hpp>
#include <boost/asio.hpp>
#include <boost/filesystem.hpp>

#include "Circle.h"
#include "CircleMap.h"
#include "Telemetry.h"

class MissionLogger
{
public:
    struct Params
    {
        boost::filesystem::path root_path = "./data/logs";
        boost::filesystem::path action_file = "actions.txt";
        boost::filesystem::path telemetry_file = "telemetry.txt";
        boost::filesystem::path map_file = "map.txt";
        boost::filesystem::path images_dir = "images";
    };

    MissionLogger(boost::asio::io_context &context,
                  const Params &params);

    void logShoot(const CircleOnMap &circle);

    void logPostShoot(const CircleOnMap &circle, cv::InputArray img);

    void logTelemetry(const Telemetry &telemetry);

    void logMap(const std::vector<CircleOnMap> &circles);

private:
    boost::asio::io_context &_context;
    const Params _params;
    const boost::filesystem::path _mission_path;
    boost::mutex _mtx;
};

#endif
