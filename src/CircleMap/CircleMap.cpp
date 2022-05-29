#include "CircleMap.h"

#include <opencv2/imgproc.hpp>

using namespace std;
using namespace cv;

void CircleMap::push(const Circle &circle)
{
    _circles.push_back(circle);
}

const std::vector<CircleMap::Circle>& CircleMap::getAll()
{
    return _circles;
}

void CircleMap::draw(InputOutputArray canvas,
                     MapLocalizer::Coords top_left,
                     MapLocalizer::Coords bottom_right)
{
    for (const auto &c : _circles)
    {
        double x = (c.coords.lng - top_left.lng) / (bottom_right.lng - top_left.lng);
        if (x < 0 || x > 1) continue;

        double y = (c.coords.lat - bottom_right.lat) / (top_left.lat - bottom_right.lat);
        if (y < 0 || y > 1) continue;

        Point px(
            static_cast<int>(canvas.size().width * x),
            static_cast<int>(canvas.size().height * (1 - y))
        );

        circle(
            canvas,
            px,
            canvas.size().width / 50,
            Scalar::all(255),
            FILLED
        );
    }
}
