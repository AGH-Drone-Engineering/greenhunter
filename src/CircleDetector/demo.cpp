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
        }

    }

    imshow("Objects", img);

    waitKey();

    return 0;
}
