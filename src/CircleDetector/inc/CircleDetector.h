#ifndef GREENHUNTER_CIRCLE_DETECTOR_H
#define GREENHUNTER_CIRCLE_DETECTOR_H

#include <opencv2/core.hpp>

class CircleDetector
{
public:

    void getColorMasks(cv::InputArray src,
                       cv::OutputArray brown,
                       cv::OutputArray gold,
                       cv::OutputArray beige);

private:

};

#endif
