#include "Hunter.h"

#include <iostream>
#include <functional>

using std::cout;
using std::endl;
using namespace std::placeholders;

namespace b = boost;
namespace ba = boost::asio;
namespace bg = boost::geometry;

Hunter::Hunter(ba::io_context &context,
               const Hunter::Params &params)
    : _params(params)
    , _map(context,
           std::bind(&Hunter::onMapUpdate, this, _1),
           params.map)
    , _mav(context,
           std::bind(&Hunter::onArrived, this),
           std::bind(&Hunter::onShot, this),
           std::bind(&Hunter::onPosition, this, _1),
           params.mav)
   , _state(State::IDLE)
{

}

void Hunter::dispatchMove()
{
    switch (_state)
    {
        case State::IDLE:
        case State::GOTO:
            goToNearest();
            break;

        default:
            break;
    }
}

void Hunter::goToNearest()
{
    auto nearest = getNearest();
    if (!nearest) return;

    if (_current_target &&
        bg::distance(
            _current_target->position,
            nearest->position
        ) < _params.reroute_min_dist)
        return;

    cout << "[Hunter] On route to new target"
         << endl;

    _current_target = *nearest;
    _mav.sendGoTo(nearest->position);
    _state = State::GOTO;
}

void Hunter::onMapUpdate(const std::vector<CircleOnMap> &circles)
{
    b::lock_guard lock(_mtx);
    _targets = filterNotHit(circles);
    dispatchMove();
}

void Hunter::onArrived()
{
    b::lock_guard lock(_mtx);

    cout << "[Hunter] Arrived at waypoint"
         << endl;

    switch (_state)
    {
        case State::GOTO:
            // arrived at target
            _state = State::GET_CORRECTION;
            _state_cond.notify_one();
            break;

        case State::APPLY_CORRECTION:
            // executed correction
            cout << "[Hunter] Shooting"
                 << endl;
            _mav.sendShoot(_current_target->color);
            _shots.push_back(_current_target->position);
            _targets = filterNotHit(_targets);
            _state = State::SHOOT;
            _current_target = b::none;
            break;

        default:
            break;
    }
}

void Hunter::onShot()
{
    b::lock_guard lock(_mtx);

    if (_state == State::SHOOT)
    {
        cout << "[Hunter] Shot complete"
             << endl;
        _state = State::IDLE;
    }
}

void Hunter::onPosition(const Telemetry &telemetry)
{
    b::lock_guard lock(_mtx);
    _telemetry = telemetry;
    dispatchMove();
}

void Hunter::run()
{
    for (;;)
    {
        b::unique_lock lock(_mtx);

        while (_state != State::GET_CORRECTION)
            _state_cond.wait(lock);

        cout << "[Hunter] Calculating correction"
             << endl;

        // TODO correction

        cout << "[Hunter] Executing correction"
             << endl;

        _mav.sendGoTo(_current_target->position);
        _state = State::APPLY_CORRECTION;
    }
}

b::optional<CircleOnMap> Hunter::getNearest() const
{
    if (!_telemetry || _targets.empty())
        return b::none;

    auto nearest = _targets.cbegin();
    double nearest_dist = bg::distance(
        nearest->position,
        _telemetry->position
    );

    for (auto c = ++_targets.cbegin(); c != _targets.cend(); ++c)
    {
        double dist = bg::distance(
            c->position,
            _telemetry->position
        );
        if (dist < nearest_dist)
        {
            nearest = c;
            nearest_dist = dist;
        }
    }

    return *nearest;
}

std::vector<CircleOnMap>
Hunter::filterNotHit(const std::vector<CircleOnMap> &targets) const
{
    std::vector<CircleOnMap> out;
    for (const auto &c : targets)
    {
        bool hit = false;
        for (const auto &s : _shots)
        {
            if (bg::distance(c.position, s) < _params.shot_dist_threshold)
            {
                hit = true;
                break;
            }
        }
        if (!hit) out.push_back(c);
    }
    return out;
}
