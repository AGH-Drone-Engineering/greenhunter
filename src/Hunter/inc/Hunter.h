#ifndef GREENHUNTER_HUNTER_H
#define GREENHUNTER_HUNTER_H

#include <vector>
#include <boost/asio.hpp>
#include <boost/optional.hpp>
#include <boost/thread.hpp>
#include <opencv2/videoio.hpp>

#include "Telemetry.h"
#include "MapClient.h"
#include "MavClient.h"
#include "CircleDetector.h"
#include "MapLocalizer.h"

class Hunter
{
public:
    typedef CircleOnMap::LatLon LatLon;

    struct Params
    {
        MapClient::Params map;
        MavClient::Params mav;
        double reroute_min_dist = 0.5;
        double shot_dist_threshold = 2.;
        int correction_tries = 5;
        CameraParams camera;
    };

    Hunter(boost::asio::io_context &context,
           const std::string &camera,
           const Params &params);

    Hunter(boost::asio::io_context &context,
           int camera,
           const Params &params);

    void run();

private:

    enum class State
    {
        IDLE,
        GOTO,
        GET_CORRECTION,
        APPLY_CORRECTION,
        SHOOT,
    };

    void onMapUpdate(const std::vector<CircleOnMap> &circles);

    void onArrived();

    void onShot();

    void onPosition(const Telemetry &telemetry);

    void dispatchMove();

    void goToNearest();

    boost::optional<CircleOnMap> getNearest() const;

    std::vector<CircleOnMap>
    filterNotHit(const std::vector<CircleOnMap> &targets) const;

    const Params _params;

    boost::mutex _mtx;
    boost::condition_variable _state_cond;

    MapClient _map;
    MavClient _mav;
    CircleDetector _detector;
    MapLocalizer _localizer;
    cv::VideoCapture _cap;

    boost::optional<Telemetry> _telemetry;
    std::vector<CircleOnMap> _targets;
    std::vector<LatLon> _shots;
    boost::optional<CircleOnMap> _current_target;

    State _state;
};

#endif
