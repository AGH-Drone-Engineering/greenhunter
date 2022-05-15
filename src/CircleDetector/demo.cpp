#include <iostream>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>

#include "CircleDetector.h"

using namespace std;
using namespace cv;

int main(int argc, char **argv)
{
    CircleDetector circleDetector;

    for (int img_i = 1; img_i <= 6; ++img_i)
    {
        Mat img_raw = imread("data/im" + to_string(img_i) + ".jpeg", IMREAD_COLOR);

        Mat img_hsv;
        cvtColor(img_raw, img_hsv, COLOR_BGR2HSV_FULL);

        Mat mask_brown, mask_gold, mask_beige;

        circleDetector.getColorMasks(img_hsv, mask_brown, mask_gold, mask_beige);

        imshow("brown", mask_brown);
        imshow("gold", mask_gold);
        imshow("beige", mask_beige);

        waitKey();
    }

    return 0;
}
