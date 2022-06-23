#include <iostream>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>

#include "CircleDetector.h"
#include "CircleSquareDetector.h"

using namespace std;
using namespace cv;

int main(int argc, char **argv)
{
    CameraParams camera;
    camera.fov_h = 60. * boost::geometry::math::d2r<double>();
    camera.fov_v = 36. * boost::geometry::math::d2r<double>();
    camera.frame_width = 1280;
    camera.frame_height = 720;
    CircleSquareDetector circleDetector({}, camera);

    Mat img = imread("data/im8.jpeg", IMREAD_COLOR);

    auto circles = circleDetector.detectCircles(img, 20.);

    for (const auto &c : circles)
    {
        ellipse(img, c.ellipse, Scalar(0, 0, 255), 2);
        switch (c.color)
        {
            case CircleColor::Brown:
                putText(img, "Brown", c.ellipse.boundingRect().tl(), FONT_HERSHEY_SIMPLEX, 1, Scalar(0, 0, 255));
                break;
            case CircleColor::Gold:
                putText(img, "Gold", c.ellipse.boundingRect().tl(), FONT_HERSHEY_SIMPLEX, 1, Scalar(0, 0, 255));
                break;
            case CircleColor::Beige:
                putText(img, "Beige", c.ellipse.boundingRect().tl(), FONT_HERSHEY_SIMPLEX, 1, Scalar(0, 0, 255));
                break;
            case CircleColor::WhiteSquare:
                putText(img, "WhiteSquare", c.ellipse.boundingRect().tl(), FONT_HERSHEY_SIMPLEX, 1, Scalar(0, 0, 255));
                break;
        }

    }

    imshow("Objects", img);

    waitKey();

    return 0;
}