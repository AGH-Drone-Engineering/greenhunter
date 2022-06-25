#include "Hunter.h"

#include <iostream>
#include <functional>

using std::cout;
using std::cerr;
using std::endl;
using namespace std::placeholders;

namespace b = boost;
namespace ba = boost::asio;
namespace bg = boost::geometry;

Hunter::Hunter(ba::io_context &context,
               const std::string &camera,
               const Hunter::Params &params)
    : _params(params)
    , _logger(context, params.logger)
    , _map(context,
           std::bind(&Hunter::onMapUpdate, this, _1),
           params.map)
    , _mav(context,
           std::bind(&Hunter::onArrived, this),
           std::bind(&Hunter::onShot, this),
           std::bind(&Hunter::onPosition, this, _1),
           params.mav)
   , _detector(params.detector, params.camera)
   , _cap(camera)
   , _state(State::IDLE)
{
    _cap.set(cv::CAP_PROP_FRAME_WIDTH, params.camera.frame_width);
    _cap.set(cv::CAP_PROP_FRAME_HEIGHT, params.camera.frame_height);
}

Hunter::Hunter(ba::io_context &context,
               int camera,
               const Hunter::Params &params)
    : _params(params)
    , _logger(context, params.logger)
    , _map(context,
           std::bind(&Hunter::onMapUpdate, this, _1),
           params.map)
    , _mav(context,
           std::bind(&Hunter::onArrived, this),
           std::bind(&Hunter::onShot, this),
           std::bind(&Hunter::onPosition, this, _1),
           params.mav)
    , _detector(params.detector, params.camera)
    , _cap(camera)
    , _state(State::IDLE)
{
    _cap.set(cv::CAP_PROP_FRAME_WIDTH, params.camera.frame_width);
    _cap.set(cv::CAP_PROP_FRAME_HEIGHT, params.camera.frame_height);
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

    _logger.logAction("GOTO", *nearest);
}

void Hunter::onMapUpdate(const std::vector<CircleOnMap> &circles)
{
    try
    {
        b::lock_guard lock(_mtx);
        _logger.logMap(circles);
        _targets = filterTargets(circles);
    }
    catch (const std::exception &ex)
    {
        cerr << "[Hunter] onMapUpdate: "
             << ex.what()
             << endl;
    }
}

void Hunter::onArrived()
{

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
    try
    {
        b::lock_guard lock(_mtx);
        _telemetry = telemetry;
        _telemetry_cond.notify_one();
        _logger.logTelemetry(telemetry);
    }
    catch (const std::exception &ex)
    {
        cerr << "[Hunter] onPosition: "
             << ex.what()
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
Hunter::filterTargets(const std::vector<CircleOnMap> &circles) const
{
    std::vector<CircleOnMap> out;
    for (const auto &c : circles)
    {
        if (c.color != CircleColor::Gold &&
            c.color != CircleColor::Beige)
            continue;

        bool hit = false;
        for (const auto &s : _visited)
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

void Hunter::run()
{
    for (;;)
    {
        try
        {
            mainLoop();
        }
        catch (const std::exception &ex)
        {
            cerr << "[Hunter] mainLoop: "
                 << ex.what()
                 << endl;
            b::lock_guard lock(_mtx);
            reset();
        }
    }
}

void Hunter::mainLoop()
{
    {
        b::unique_lock lock(_mtx);
        _telemetry_cond.wait(lock);
        dispatchMove();
        if (_state != State::APPROACH)
            return;
        _logger.logAction("AIM", *_current_target);
    }

    cout << "[Hunter] Starting visual approach"
         << endl;

    _aim_retries = _params.correction_tries;

    while (!aimLoop());

    cout << "[Hunter] Approach complete"
         << endl;
}

bool Hunter::aimLoop()
{
    Telemetry telem;

    {
        b::unique_lock lock(_mtx);
        _telemetry_cond.wait(lock);
        telem = *_telemetry;
    }

    cv::Mat frame;
    _cap.read(frame);

    const auto circles = _detector.detectCircles(frame, telem.altitude);

    if (circles.empty())
    {
        if (!--_aim_retries)
        {
            cout << "[Hunter] Could not find target"
                 << endl;
            {
                b::unique_lock lock(_mtx);
                _logger.logAction("MISS", *_current_target);
                markVisited(_current_target->position);
                reset();
            }
            return true;
        }
        return false;
    }

    _aim_retries = _params.correction_tries;

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
        b::unique_lock lock(_mtx);
        cout << "[Hunter] Shooting"
             << endl;
        _mav.sendShoot(_current_target->color);
        _logger.logCircleImage(*_current_target, frame);
        _logger.logAction("SHOOT", *_current_target);
        markVisited(_current_target->position);
        _state = State::SHOOT;
        _current_target = b::none;
        return true;
    }
    else
    {
        {
            b::unique_lock lock(_mtx);
            _current_target->position = target.position;
        }
        _mav.sendGoTo(target.position);
    }

    return false;
}

void Hunter::reset()
{
    _state = State::IDLE;
    _current_target = b::none;
}

void Hunter::markVisited(const Position &position)
{
    _visited.push_back(position);
    _targets = filterTargets(_targets);
}
