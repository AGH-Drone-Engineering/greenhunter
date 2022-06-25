#include "CircleSquareDetector.h"

#include <opencv2/imgproc.hpp>

using namespace cv;
using std::vector;
using std::cerr;
using std::endl;
using boost::math::constants::pi;

CircleSquareDetector::CircleSquareDetector(const Config &config,
                                           const CameraParams &camera)
    : _config(config)
    , _camera(camera)
{

}

vector<CircleOnFrame> CircleSquareDetector::detectCircles(InputArray src, double altitude) const
{
    try
    {
        Mat img;

        double ground_width = 2. * altitude * tan(_camera.fov_h * 0.5);
        double ground_height = 2. * altitude * tan(_camera.fov_v * 0.5);

        double fx = _config.px_per_m / src.size().width * ground_width;
        double fy = _config.px_per_m / src.size().height * ground_height;

        resize(src, img, Size(), fx, fy, INTER_AREA);

        for (int i = 0; i < _config.medianIters; ++i)
        {
            medianBlur(img, img, _config.medianSize);
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
            if (hier[i][3] >= 0) continue;
            if (cnts[i].size() < 5) continue;

            double cnt_area = contourArea(cnts[i]);
            double cnt_peri = arcLength(cnts[i], true);
            double circularity = 4. * pi<double>() * cnt_area / cnt_peri / cnt_peri;

            vector<Point> hull;
            convexHull(cnts[i], hull);
            double hull_area = contourArea(hull);
            double convexity = cnt_area / hull_area;

            RotatedRect fit_ellipse = fitEllipse(cnts[i]);
            RotatedRect fit_rect = minAreaRect(cnts[i]);

            double square_ratio = cnt_area / fit_rect.size.area();

            if (fit_ellipse.size.width < _config.predSizeMin) continue;
            if (fit_ellipse.size.height < _config.predSizeMin) continue;
            if (cnt_area < _config.predAreaMin) continue;
            if (convexity < _config.predConvexityMin) continue;

            bool is_square = false;

            if (circularity < _config.predCircularityMin)
            {
                if (square_ratio > _config.predSquareRatioMin)
                {
                    is_square = true;
                    fit_ellipse = fit_rect;
                }
                else continue;
            }

            Rect obj_rect = fit_ellipse.boundingRect() & Rect(0, 0, img.cols, img.rows);

            Mat obj_mask = Mat::zeros(obj_rect.size(), img.type());
            ellipse(
                obj_mask,
                RotatedRect(
                    fit_ellipse.center - Point2f(obj_rect.tl()),
                    fit_ellipse.size,
                    fit_ellipse.angle
                ),
                Scalar(255, 255, 255),
                FILLED
            );
            erode(obj_mask, obj_mask, getStructuringElement(MORPH_ELLIPSE, Size(5, 5)));

            Mat obj_img;
            img(obj_rect).copyTo(obj_img, obj_mask);

            cvtColor(obj_img, obj_img, COLOR_BGR2HSV_FULL);

            const auto mean_color = mean(obj_img);
            CircleColor circle_color;

            if (is_square)
            {
                if (mean_color[1] / 255.f < _config.beigeSatMax)
                    circle_color = CircleColor::WhiteSquare;
                else
                    circle_color = CircleColor::Brown;
            }
            else
            {
                if (mean_color[1] / 255.f < _config.beigeSatMax)
                    circle_color = CircleColor::Beige;
                else
                    circle_color = CircleColor::Gold;
            }

            fit_ellipse.center.x /= fx;
            fit_ellipse.center.y /= fy;
            fit_ellipse.size.width /= fx;
            fit_ellipse.size.height /= fy;

            circles.push_back({circle_color, fit_ellipse});
        }

        return circles;
    }
    catch (const std::exception &ex)
    {
        cerr << "[CircleSquareDetector] "
             << ex.what()
             << endl;
        return {};
    }
}
