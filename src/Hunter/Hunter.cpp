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
               const std::string &camera,
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
   , _cap(camera)
   , _state(State::IDLE)
{

}

Hunter::Hunter(ba::io_context &context,
               int camera,
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
    , _cap(camera)
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

    double distance = bg::distance(
        _telemetry->position,
        nearest->position
    );

    if (distance < _params.approach_dist)
    {
        _current_target = *nearest;
        _state = State::APPROACH;
        _approach_cond.notify_one();
        return;
    }

    if (_current_target &&
        bg::distance(
            _current_target->position,
            nearest->position
        ) < _params.reroute_dist)
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
}

void Hunter::onArrived()
{
    b::lock_guard lock(_mtx);

    cout << "[Hunter] Arrived at waypoint"
         << endl;
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
    _telemetry_cond.notify_one();
    dispatchMove();
}

void Hunter::run()
{
    for (;;)
    {
        b::unique_lock lock(_mtx);

        while (_state != State::APPROACH)
            _approach_cond.wait(lock);

        lock.unlock();

        cout << "[Hunter] Starting visual approach"
             << endl;

        cv::Mat frame;

        int retries = _params.correction_tries;

        for (;;)
        {
            lock.lock();
            _telemetry_cond.wait(lock);
            const auto telem = *_telemetry;
            lock.unlock();

            _cap.read(frame);

            const auto circles = _detector.detectCircles(frame);
            if (circles.empty())
            {
                if (!--retries)
                {
                    cout << "[Hunter] Could not find target"
                         << endl;
                    lock.lock();
                    _shots.push_back(_current_target->position);
                    _targets = filterNotHit(_targets);
                    _state = State::IDLE;
                    _current_target = b::none;
                    lock.unlock();
                    break;
                }
                continue;
            }

            retries = _params.correction_tries;

            cv::Point2f center(
                frame.size().width * 0.5f,
                frame.size().height * 0.5f
            );

            auto nearest = circles.cbegin();
            double nearest_dist = cv::norm(
                nearest->ellipse.center - center
            );

            for (auto c = ++circles.cbegin(); c != circles.cend(); ++c)
            {
                double dist = cv::norm(
                    c->ellipse.center - center
                );
                if (dist < nearest_dist)
                {
                    nearest = c;
                    nearest_dist = dist;
                }
            }

            auto target = _localizer.localize(
                *nearest,
                telem,
                _params.camera
            );

            if (bg::distance(
                target.position,
                telem.position) < _params.shooting_dist)
            {
                lock.lock();
                shoot();
                lock.unlock();
                break;
            }
            else
            {
                lock.lock();
                _current_target = target;
                lock.unlock();
                _mav.sendGoTo(target.position);
            }
        }

        cout << "[Hunter] Approach complete"
             << endl;
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
            if (bg::distance(c.position, s) < _params.visited_dist)
            {
                hit = true;
                break;
            }
        }
        if (!hit) out.push_back(c);
    }
    return out;
}

void Hunter::shoot()
{
    cout << "[Hunter] Shooting"
         << endl;
    _mav.sendShoot(_current_target->color);
    _shots.push_back(_current_target->position);
    _targets = filterNotHit(_targets);
    _state = State::SHOOT;
    _current_target = b::none;
}
