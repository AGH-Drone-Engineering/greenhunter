#include "Hunter.h"

#include <iostream>
#include <functional>

using std::cout;
using std::endl;
using namespace std::placeholders;
typedef CircleOnMap::LatLon LatLon;

namespace b = boost;
namespace ba = boost::asio;

Hunter::Hunter(ba::io_context &context,
               const Hunter::Params &params)
    : _map(context,
           std::bind(&Hunter::onMapUpdate, this, _1),
           params.map)
    , _mav(context,
           std::bind(&Hunter::onArrived, this),
           std::bind(&Hunter::onShot, this),
           std::bind(&Hunter::onPosition, this, _1),
           params.mav)
{

}

void Hunter::onMapUpdate(const std::vector<CircleOnMap> &circles)
{
    cout << "[Hunter] "
         << circles.size()
         << " circles on map"
         << endl;
}

void Hunter::onArrived()
{
    cout << "[Hunter] Arrived at waypoint"
         << endl;
}

void Hunter::onShot()
{
    cout << "[Hunter] Shot complete"
         << endl;
}

void Hunter::onPosition(const Telemetry &telemetry)
{
    _telemetry = telemetry;
    cout << "[Hunter] Telemetry: "
         << telemetry.position.get<1>() * b::geometry::math::r2d<double>()
         << " "
         << telemetry.position.get<0>() * b::geometry::math::r2d<double>()
         << " "
         << telemetry.azimuth * b::geometry::math::r2d<double>()
         << " "
         << telemetry.altitude
         << endl;
}
