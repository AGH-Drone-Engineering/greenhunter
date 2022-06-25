#ifndef GREENHUNTER_MISSION_LOGGER_H
#define GREENHUNTER_MISSION_LOGGER_H

#include <string>
#include <vector>
#include <chrono>
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
        boost::filesystem::path flight_img_dir = "flight";
        boost::filesystem::path circle_img_dir = "circles";
    };

    MissionLogger(boost::asio::io_context &context,
                  const Params &params);

    void logAction(const std::string &action, const CircleOnMap &circle);

    void logCircleImage(const CircleOnMap &circle, cv::InputArray img);

    void logFlightImage(cv::InputArray img);

    void logTelemetry(const Telemetry &telemetry);

    void logMap(const std::vector<CircleOnMap> &circles);

private:
    std::chrono::milliseconds getTimestamp() const;

    boost::asio::io_context &_context;
    const Params _params;
    const boost::filesystem::path _mission_path;
    const std::chrono::steady_clock::time_point _mission_start;
    boost::mutex _mtx;
    long _n_flight_images;
};

#endif
