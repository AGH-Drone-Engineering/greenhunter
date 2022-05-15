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

        Mat img_cnt_brown = img_raw.clone();
        Mat img_cnt_gold = img_raw.clone();
        Mat img_cnt_beige = img_raw.clone();

        vector<vector<Point>> cnt_brown;
        findContours(mask_brown, cnt_brown, RETR_LIST, CHAIN_APPROX_SIMPLE);
        drawContours(img_cnt_brown, cnt_brown, -1, Scalar(0, 0, 255));

        vector<vector<Point>> cnt_gold;
        findContours(mask_gold, cnt_gold, RETR_LIST, CHAIN_APPROX_SIMPLE);
        drawContours(img_cnt_gold, cnt_gold, -1, Scalar(0, 0, 255));

        vector<vector<Point>> cnt_beige;
        findContours(mask_beige, cnt_beige, RETR_LIST, CHAIN_APPROX_SIMPLE);
        drawContours(img_cnt_beige, cnt_beige, -1, Scalar(0, 0, 255));

        imshow("brown", img_cnt_brown);
        imshow("gold", img_cnt_gold);
        imshow("beige", img_cnt_beige);

        waitKey();
    }

    return 0;
}
