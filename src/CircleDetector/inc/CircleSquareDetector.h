#ifndef GREENHUNTER_CIRCLE_SQUARE_DETECTOR_H
#define GREENHUNTER_CIRCLE_SQUARE_DETECTOR_H

#include <opencv2/core.hpp>
#include <vector>

#include "Circle.h"
#include "CameraParams.h"

class CircleSquareDetector
{
public:
    struct Config
    {
        int medianSize = 5;
        int medianIters = 2;

        double cannyTh1 = 150;
        double cannyTh2 = 200;

        int closeSize = 3;
        int closeIters = 2;

        double predSizeMin = 35;
        double predAreaMin = 962;
        double predCircularityMin = 0.85;
        double predConvexityMin = 0.9;
        double predSquareRatioMin = 0.85;

        double beigeSatMax = 0.1;

        double px_per_m = 50.;
    };

    CircleSquareDetector(const Config &config, const CameraParams &camera);

    std::vector<CircleOnFrame> detectCircles(cv::InputArray src, double altitude) const;

private:

    Config _config;
    CameraParams _camera;

};

#endif
