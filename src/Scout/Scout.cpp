#include "Scout.h"

#include <iostream>

#include <opencv2/imgproc.hpp>
#include <opencv2/highgui.hpp>

using namespace std;
using namespace cv;

Scout::Scout(boost::asio::io_context &io_context, const Params &params)
    : _drone(io_context, params.drone_params)
    , _map_server(io_context, _map, params.map_port)
{}

void Scout::run()
{
    auto frameTelemetry = _drone.getFrameWithTelemetry();
    auto frameCircles = _detector.detectCircles(frameTelemetry.frame);

    Mat canvas = frameTelemetry.frame.clone();
    for (const auto &c : frameCircles)
    {
        ellipse(canvas, c.ellipse, Scalar(0, 0, 255), 2);
        switch (c.type)
        {
            case CircleDetector::CircleType::Brown:
                putText(canvas, "Brown", c.ellipse.boundingRect().tl(), FONT_HERSHEY_SIMPLEX, 1, Scalar(0, 0, 255));
                break;
            case CircleDetector::CircleType::Gold:
                putText(canvas, "Gold", c.ellipse.boundingRect().tl(), FONT_HERSHEY_SIMPLEX, 1, Scalar(0, 0, 255));
                break;
            case CircleDetector::CircleType::Beige:
                putText(canvas, "Beige", c.ellipse.boundingRect().tl(), FONT_HERSHEY_SIMPLEX, 1, Scalar(0, 0, 255));
                break;
        }
    }
    namedWindow("Detections", WINDOW_NORMAL);
    imshow("Detections", canvas);

    for (const auto &fc : frameCircles)
    {
        const auto coords = _localizer.localize(
            fc.ellipse.center,
            frameTelemetry.telemetry,
            frameTelemetry.camera
        );

        CircleMap::Circle mc = {
            coords
        };

        _map.push(mc);
    }

    cout << "Circles on map: " << _map.getAll().size() << endl;

    canvas = Mat::zeros(1024, 1024, CV_8UC1);
    _map.draw(canvas);
    namedWindow("Map", WINDOW_NORMAL);
    imshow("Map", canvas);

    waitKey();
}
