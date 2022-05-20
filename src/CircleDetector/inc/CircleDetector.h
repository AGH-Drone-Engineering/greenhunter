#ifndef GREENHUNTER_CIRCLE_DETECTOR_H
#define GREENHUNTER_CIRCLE_DETECTOR_H

#include <opencv2/core.hpp>
#include <vector>

class CircleDetector
{
public:
    struct Config
    {
        int medianSize = 9;
        int medianIters = 3;

        double cannyTh1 = 150;
        double cannyTh2 = 200;

        int closeSize = 9;
        int closeIters = 1;

        double predSizeMin = 10;
        double predAreaMin = 200;
        double predCircularityMin = 0.7;
        double predConvexityMin = 0.9;
    };

    enum class CircleType
    {
        Brown,
        Gold,
        Beige,
    };

    struct Circle
    {
        CircleType type;
        cv::RotatedRect ellipse;
    };

    std::vector<Circle> detectCircles(cv::InputArray src);

private:

    Config _config;

};

#endif
