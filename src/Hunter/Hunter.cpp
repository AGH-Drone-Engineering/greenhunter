#include "Hunter.h"

#include <iostream>
#include <functional>

using std::cout;
using std::endl;
using namespace std::placeholders;

namespace b = boost;
namespace ba = boost::asio;

Hunter::Hunter(ba::io_context &context,
               const Hunter::Params &params)
    : _map(context,
           std::bind(&Hunter::onMapUpdate, this, _1),
           params.map)
{

}

void Hunter::onMapUpdate(const std::vector<CircleOnMap> &circles)
{
    cout << "Circles on map:" << endl;
    for (const auto &c : circles)
    {
        cout << c.position.get<0>() << " "
             << c.position.get<1>() << " "
             << static_cast<int>(c.color)
             << endl;
    }
}
