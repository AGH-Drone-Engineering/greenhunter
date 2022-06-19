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
           std::bind(&Hunter::onPosition, this, _1, _2, _3),
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

void Hunter::onPosition(const LatLon &pos, double azi, double alt)
{
    cout << "[Hunter] LGHA: "
         << pos.get<0>()
         << " "
         << pos.get<1>()
         << " "
         << azi
         << " "
         << alt
         << endl;
}
