#include "CircleDetector.h"

#include <opencv2/imgproc.hpp>

using namespace cv;
using namespace std;

vector<CircleOnFrame> CircleDetector::detectCircles(InputArray src) const
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

    vector<CircleOnFrame> circles;

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

        Rect obj_rect = obj.boundingRect() & Rect(0, 0, img.cols, img.rows);

        Mat obj_mask = Mat::zeros(obj_rect.size(), img.type());
        ellipse(
            obj_mask,
            RotatedRect(
                obj.center - Point2f(obj_rect.tl()),
                obj.size,
                obj.angle
            ),
            Scalar(255, 255, 255),
            FILLED
        );
        erode(obj_mask, obj_mask, getStructuringElement(MORPH_ELLIPSE, Size(5, 5)));

        Mat obj_img;
        img(obj_rect).copyTo(obj_img, obj_mask);

        cvtColor(obj_img, obj_img, COLOR_BGR2HSV_FULL);

        Mat err_brown, err_gold, err_beige;
        absdiff(obj_img, _config.brown, err_brown);
        absdiff(obj_img, _config.gold, err_gold);
        absdiff(obj_img, _config.beige, err_beige);

        Mat err_h, err_s, err_v;

        extractChannel(err_brown, err_h, 0);
        extractChannel(err_brown, err_s, 1);
        extractChannel(err_brown, err_v, 2);
//        err_brown = err_h + err_s + err_v;
        add(err_h, err_s, err_brown);
        add(err_brown, err_v, err_brown);

        extractChannel(err_gold, err_h, 0);
        extractChannel(err_gold, err_s, 1);
        extractChannel(err_gold, err_v, 2);
//        err_gold = err_h + err_s + err_v;
        add(err_h, err_s, err_gold);
        add(err_gold, err_v, err_gold);

        extractChannel(err_beige, err_h, 0);
        extractChannel(err_beige, err_s, 1);
        extractChannel(err_beige, err_v, 2);
//        err_beige = err_h + err_s + err_v;
        add(err_h, err_s, err_beige);
        add(err_beige, err_v, err_beige);

        double err_brown_mean = mean(err_brown)[0];
        double err_gold_mean = mean(err_gold)[0];
        double err_beige_mean = mean(err_beige)[0];

        if (err_brown_mean <= err_gold_mean && err_gold_mean <= err_beige_mean)
            circles.push_back({CircleColor::Brown, obj});
        else if (err_gold_mean <= err_brown_mean && err_brown_mean <= err_beige_mean)
            circles.push_back({CircleColor::Gold, obj});
        else
            circles.push_back({CircleColor::Beige, obj});
    }

    return circles;
}
