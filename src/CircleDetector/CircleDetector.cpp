#include "CircleDetector.h"

using namespace cv;

#define BROWN_LO Scalar(28. / 360 * 255, 0.38 * 255, 0.7 * 255)
#define  BROWN_HI Scalar(32.5 / 360 * 255, 0.55 * 255, 0.84 * 255)

#define  GOLD_LO Scalar(38.5 / 360 * 255, 0.7 * 255, 0.84 * 255)
#define  GOLD_HI Scalar(42.5 / 360 * 255, 0.8 * 255, 0.91 * 255)

#define  BEIGE_LO Scalar(48. / 360 * 255, 0.05 * 255, 0.8 * 255)
#define  BEIGE_HI Scalar(65. / 360 * 255, 0.16 * 255, 0.89 * 255)

void CircleDetector::getColorMasks(InputArray src,
                                   OutputArray brown,
                                   OutputArray gold,
                                   OutputArray beige)
{
    inRange(src, BROWN_LO, BROWN_HI, brown);
    inRange(src, GOLD_LO, GOLD_HI, gold);
    inRange(src, BEIGE_LO, BEIGE_HI, beige);
}
