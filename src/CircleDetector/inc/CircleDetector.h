#ifndef GREENHUNTER_CIRCLE_DETECTOR_H
#define GREENHUNTER_CIRCLE_DETECTOR_H

#include <opencv2/core.hpp>
#include <vector>

#include "Circle.h"

class CircleDetector
{
public:
    struct Config
    {
        int medianSize = 5;
        int medianIters = 10;

        double cannyTh1 = 150;
        double cannyTh2 = 200;

        int closeSize = 9;
        int closeIters = 1;

        double predSizeMin = 35;
        double predAreaMin = 962;
        double predCircularityMin = 0.7;
        double predConvexityMin = 0.9;

        cv::Scalar brown = {26.197183098591555 / 360. * 255., 0.48299319727891155 * 255., 0.5764705882352941 * 255.};
        cv::Scalar gold = {38.367346938775505 / 360. * 255., 0.6933962264150944 * 255., 0.8313725490196079 * 255.};
        cv::Scalar beige = {51.81818181818184 / 360. * 255., 0.0883534136546185 * 255., 0.9764705882352941 * 255.};
    };

    std::vector<CircleOnFrame> detectCircles(cv::InputArray src) const;

private:

    Config _config;

};

#endif
