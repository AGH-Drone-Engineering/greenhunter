#include "Scout.h"

#include <iostream>

#include <opencv2/imgproc.hpp>
#include <opencv2/highgui.hpp>

using namespace std;
using namespace cv;

Scout::Scout(boost::asio::io_context &io_context,
             const string &camera,
             const Params &params)
    : _params(params)
    , _drone(io_context, camera, params.drone)
    , _detector(params.detector, params.drone.camera)
    , _map(params.map)
    , _map_server(io_context, _map, params.map_port)
    , _logger(io_context, params.logger)
{}

Scout::Scout(boost::asio::io_context &io_context,
             int camera,
             const Params &params)
    : _params(params)
    , _drone(io_context, camera, params.drone)
    , _detector(params.detector, params.drone.camera)
    , _map(params.map)
    , _map_server(io_context, _map, params.map_port)
    , _logger(io_context, params.logger)
{}

void Scout::run()
{
    namedWindow("Detections", WINDOW_KEEPRATIO);
    namedWindow("Map", WINDOW_KEEPRATIO);

    for (;;)
    {
        try
        {
            if (mainLoop())
                break;
        }
        catch (const std::exception &ex)
        {
            cerr << "[Scout] mainLoop: "
                 << ex.what()
                 << endl;
        }
    }
}

bool Scout::mainLoop()
{
    _drone.waitInAir(_params.min_altitude);
    auto frameTelemetryOpt = _drone.getFrameWithTelemetry();
    if (!frameTelemetryOpt)
    {
        _drone.waitTelemetryValid();
        return false;
    }
    auto frameTelemetry = *frameTelemetryOpt;

    auto frameCircles = _detector.detectCircles(frameTelemetry.frame, frameTelemetry.telemetry.altitude);

    for (const auto &c : frameCircles)
    {
        _map.push(_localizer.localize(
            c,
            frameTelemetry.telemetry,
            _params.drone.camera
        ));
    }

    try
    {
        _logger.logMap(_map.getAll());
        _logger.logTelemetry(frameTelemetry.telemetry);
        _logger.logFlightImage(frameTelemetry.frame);
    }
    catch (const std::exception &ex)
    {
        cerr << "[Scout] Log error: "
             << ex.what()
             << endl;
    }

    try
    {
        Mat canvas = frameTelemetry.frame.clone();
        for (const auto &c : frameCircles)
        {
            ellipse(canvas, c.ellipse, Scalar(0, 0, 255), 2);
            switch (c.color)
            {
                case CircleColor::Brown:
                    putText(canvas, "Brown", c.ellipse.boundingRect().tl(), FONT_HERSHEY_SIMPLEX, 1, Scalar(0, 0, 255));
                    break;
                case CircleColor::Gold:
                    putText(canvas, "Gold", c.ellipse.boundingRect().tl(), FONT_HERSHEY_SIMPLEX, 1, Scalar(0, 0, 255));
                    break;
                case CircleColor::Beige:
                    putText(canvas, "Beige", c.ellipse.boundingRect().tl(), FONT_HERSHEY_SIMPLEX, 1, Scalar(0, 0, 255));
                    break;
                case CircleColor::WhiteSquare:
                    putText(canvas, "WhiteSquare", c.ellipse.boundingRect().tl(), FONT_HERSHEY_SIMPLEX, 1, Scalar(0, 0, 255));
                    break;
            }
        }
        imshow("Detections", canvas);

        canvas = Mat::zeros(1024, 1024, CV_8UC3);
        _map.draw(canvas);
        imshow("Map", canvas);
    }
    catch (const std::exception &ex)
    {
        cerr << "[Scout] Drawing error: "
             << ex.what()
             << endl;
    }

    return waitKey(1) == 'q';
}
