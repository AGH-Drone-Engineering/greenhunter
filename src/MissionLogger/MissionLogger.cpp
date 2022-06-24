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
using chrono::steady_clock;
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
    , _mission_start(steady_clock::now().time_since_epoch().count())
    , _n_flight_images(0)
{
    _context.post([this] {
        b::lock_guard lock(_mtx);
        bfs::create_directories(_mission_path / _params.flight_img_dir);
        bfs::create_directories(_mission_path / _params.circle_img_dir);

        bfs::fstream fs;

        fs.open(_mission_path / _params.action_file, bfs::fstream::out);
        fs << "timestamp,action,lat,long,color" << endl;
        fs.close();

        fs.open(_mission_path / _params.telemetry_file, bfs::fstream::out);
        fs << "timestamp,lat,long,azimuth,altitude" << endl;
        fs.close();
    });
}

void MissionLogger::logShoot(const CircleOnMap &circle)
{
    const auto t = getTimestamp();
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

void MissionLogger::logCircleImage(const CircleOnMap &circle, cv::InputArray &img)
{
    cv::Mat mat = img.getMat();
    _context.post([this, circle, mat] {
        b::lock_guard lock(_mtx);
        auto file =
            _mission_path /
            _params.circle_img_dir /
            circle_to_string(circle);
        cv::imwrite(file.string() + ".jpg", mat);
    });
}

void MissionLogger::logFlightImage(cv::InputArray &img)
{
    cv::Mat mat = img.getMat();
    _context.post([this, mat] {
        b::lock_guard lock(_mtx);
        auto file =
            _mission_path /
            _params.flight_img_dir /
            std::to_string(_n_flight_images);
        _n_flight_images++;
        cv::imwrite(file.string() + ".jpg", mat);
    });
}

void MissionLogger::logTelemetry(const Telemetry &telemetry)
{
    const auto t = getTimestamp();
    _context.post([this, t, telemetry] {
        b::lock_guard lock(_mtx);
        bfs::fstream fs;
        fs.open(_mission_path / _params.telemetry_file, bfs::fstream::app);
        fs << t << ","
           << position_to_string_deg(telemetry.position) << ","
           << telemetry.azimuth * r2d<double>() << ","
           << telemetry.altitude
           << endl;
        fs.close();
    });
}

void MissionLogger::logMap(const std::vector<CircleOnMap> &circles)
{
    _context.post([this, circles] {
        b::lock_guard lock(_mtx);
        bfs::fstream fs;
        fs.open(_mission_path / _params.map_file, bfs::fstream::out);
        fs << "lat,long,color" << "\n";
        for (const auto &c : circles)
        {
            fs << circle_to_string(c)
               << "\n";
        }
        fs.close();
    });
}

long MissionLogger::getTimestamp() const
{
    return steady_clock::now().time_since_epoch().count() - _mission_start;
}
