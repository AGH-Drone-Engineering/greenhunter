#include "CircleDetector.h"

#include <opencv2/imgproc.hpp>

using namespace cv;
using namespace std;

vector<CircleDetector::Circle> CircleDetector::detectCircles(InputArray src)
{
    Mat img;

    for (int i = 0; i < _config.medianIters; ++i)
    {
        medianBlur(src, img, _config.medianSize);
    }

    Mat edges;
    Canny(img, edges, _config.cannyTh1, _config.cannyTh2);
    morphologyEx(
        edges,
        edges,
        MORPH_CLOSE,
        getStructuringElement(
            MORPH_ELLIPSE,
            Size(_config.closeSize, _config.closeSize)
        ),
        Point(-1, -1),
        _config.closeIters
    );

    vector<vector<Point>> cnts;
    vector<Vec4i> hier;
    findContours(edges, cnts, hier, RETR_TREE, CHAIN_APPROX_SIMPLE);

    vector<RotatedRect> objects;

    for (int i = 0; i < cnts.size(); ++i)
    {
        if (hier[i][2] >= 0) continue;
        if (cnts[i].size() < 5) continue;

        double cnt_area = contourArea(cnts[i]);
        double cnt_peri = arcLength(cnts[i], true);
        double circularity = 4. * M_PI * cnt_area / cnt_peri / cnt_peri;

        vector<Point> hull;
        convexHull(cnts[i], hull);
        double hull_area = contourArea(hull);
        double convexity = cnt_area / hull_area;

        RotatedRect obj = fitEllipse(cnts[i]);

        if (obj.size.width < _config.predSizeMin) continue;
        if (obj.size.height < _config.predSizeMin) continue;
        if (cnt_area < _config.predAreaMin) continue;
        if (circularity < _config.predCircularityMin) continue;
        if (convexity < _config.predConvexityMin) continue;

        objects.push_back(obj);
    }

    vector<Circle> circles;

    for (const auto &obj : objects)
    {
        circles.push_back({CircleType::Brown, obj});
    }

    return circles;
}
