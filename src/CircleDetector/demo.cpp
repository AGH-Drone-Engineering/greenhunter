#include <iostream>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>

#include "CircleDetector.h"

using namespace std;
using namespace cv;

int main(int argc, char **argv)
{
    CircleDetector circleDetector;

    Mat img = imread("data/im7.jpeg", IMREAD_COLOR);

    auto circles = circleDetector.detectCircles(img);

    for (const auto &c : circles)
    {
        ellipse(img, c.ellipse, Scalar(0, 0, 255), 2);
    }

    imshow("Objects", img);

    waitKey();

    return 0;
}
