#include "MissionLogger.h"

#include <opencv2/imgcodecs.hpp>
#include <iostream>
#include <chrono>

namespace b = boost;
namespace ba = b::asio;
namespace bfs = b::filesystem;
namespace chrono = std::chrono;
using boost::geometry::math::r2d;
using chrono::system_clock;
using std::to_string;
using std::endl;

MissionLogger::MissionLogger(ba::io_context &context,
                             const Params &params)
    : _context(context)
    , _params(params)
    , _mission_path(
        _params.root_path / to_string(
            system_clock::now().time_since_epoch().count()
        ))
{
    _context.post([this] {
        b::lock_guard lock(_mtx);
        bfs::create_directories(_mission_path / _params.images_dir);
    });
}

void MissionLogger::logShoot(const CircleOnMap &circle)
{
    const auto t = system_clock::now().time_since_epoch().count();
    _context.post([this, t, circle] {
        b::lock_guard lock(_mtx);
        bfs::fstream fs;
        fs.open(_mission_path / _params.action_file, bfs::fstream::app);
        fs << t << ","
           << "SHOOT,"
           << circle_to_string(circle)
           << endl;
        fs.close();
    });
}

void MissionLogger::logPostShoot(const CircleOnMap &circle, cv::InputArray &img)
{
    _context.post([this, circle, img] {
        b::lock_guard lock(_mtx);
        auto file =
            _mission_path /
            _params.images_dir /
            circle_to_string(circle);
        cv::imwrite(file.string() + ".jpg", img);
    });
}

void MissionLogger::logTelemetry(const Telemetry &telemetry)
{
    const auto t = system_clock::now().time_since_epoch().count();
    _context.post([this, t, telemetry] {
        b::lock_guard lock(_mtx);
        bfs::fstream fs;
        fs.open(_mission_path / _params.telemetry_file, bfs::fstream::app);
        fs.precision(8);
        fs << t << ","
           << telemetry.position.get<1>() * r2d<double>() << ","
           << telemetry.position.get<0>() * r2d<double>() << ","
           << telemetry.azimuth * r2d<double>() << ","
           << telemetry.altitude
           << endl;
        fs.close();
    });
}

void MissionLogger::logMap(const std::vector<CircleOnMap> &circles)
{

}
