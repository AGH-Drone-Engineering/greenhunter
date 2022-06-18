#include "Scout.h"

#include <iostream>

#include <opencv2/imgproc.hpp>
#include <opencv2/highgui.hpp>

using namespace std;
using namespace cv;

Scout::Scout(boost::asio::io_context &io_context,
             const string &camera,
             const Params &params)
    : _drone(io_context, camera, params.drone)
    , _map(params.map)
    , _map_server(io_context, _map, params.map_port)
{}

Scout::Scout(boost::asio::io_context &io_context,
             int camera,
             const Params &params)
    : _drone(io_context, camera, params.drone)
    , _map(params.map)
    , _map_server(io_context, _map, params.map_port)
{}

void Scout::run()
{
    _drone.waitTelemetryValid();

    for (;;)
    {
        auto frameTelemetryOpt = _drone.getFrameWithTelemetry();
        if (!frameTelemetryOpt) break;
        auto frameTelemetry = *frameTelemetryOpt;
        auto frameCircles = _detector.detectCircles(frameTelemetry.frame);

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
            }
        }
        namedWindow("Detections", WINDOW_NORMAL);
        imshow("Detections", canvas);

        std::for_each(
            frameCircles.cbegin(),
            frameCircles.cend(),
            [&] (const CircleOnFrame &c) {
                _map.push(_localizer.localize(c,
                    frameTelemetry.telemetry,
                    frameTelemetry.camera
                ));
            }
        );

        cout << "Circles on map: " << _map.getAll().size() << endl;

        canvas = Mat::zeros(1024, 1024, CV_8UC3);
        _map.draw(canvas);
        namedWindow("Map", WINDOW_NORMAL);
        imshow("Map", canvas);

        if (waitKey() == 'q') break;
    }
}
