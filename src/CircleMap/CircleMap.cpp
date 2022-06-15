#include "CircleMap.h"

#include <iostream>
#include <opencv2/imgproc.hpp>
#include <boost/geometry.hpp>

namespace bg = boost::geometry;
typedef bg::formula::vincenty_inverse<double, true, true> vin_inv;
typedef bg::formula::vincenty_direct<double, true> vin_dir;

void mergeClusters(CircleMap::CircleCluster &c1,
                   const CircleMap::CircleCluster &c2)
{
    auto &p1 = c1.position;
    auto p2 = c2.position;

    bg::multiply_value(p1,
       c1.brown_votes + c1.gold_votes + c1.beige_votes);
    bg::multiply_value(p2,
       c2.brown_votes + c2.gold_votes + c2.beige_votes);

    bg::add_point(p1, p2);

    bg::divide_value(
        p1,
        c1.brown_votes +
        c1.gold_votes +
        c1.beige_votes +
        c2.brown_votes +
        c2.gold_votes +
        c2.beige_votes
    );

    c1.brown_votes += c2.brown_votes;
    c1.gold_votes += c2.gold_votes;
    c1.beige_votes += c2.beige_votes;
}

CircleMap::CircleMap(const Params &params)
    : _params(params)
{

}

void CircleMap::push(const CircleOnMap &circle)
{
    std::vector<CircleCluster> to_merge;
    std::vector<CircleCluster> others;

    for (const auto &clust : _clusters)
    {
        if (bg::distance(
                circle.position,
                clust.position
            ) > _params.clustering_distance)
        {
            others.push_back(clust);
        }
        else
        {
            to_merge.push_back(clust);
        }
    }

    CircleCluster merged {
        circle.position,
        circle.color == CircleColor::Brown ? 1 : 0,
        circle.color == CircleColor::Gold ? 1 : 0,
        circle.color == CircleColor::Beige ? 1 : 0,
    };

    for (const auto &c : to_merge)
    {
        mergeClusters(merged, c);
    }

    others.push_back(merged);

    _clusters = std::move(others);
}

std::vector<CircleOnMap> CircleMap::getAll()
{
    std::vector<CircleOnMap> out;
    std::transform(
        _clusters.cbegin(),
        _clusters.cend(),
        std::back_inserter(out),
        [] (const CircleCluster &c) {
            return CircleOnMap({
                (c.brown_votes >= c.gold_votes && c.brown_votes >= c.beige_votes)
                    ? CircleColor::Brown :
                (c.gold_votes >= c.brown_votes && c.gold_votes >= c.beige_votes)
                    ? CircleColor::Gold
                    : CircleColor::Beige,
                c.position
            });
        }
    );
    return out;
}

void CircleMap::draw(cv::InputOutputArray canvas)
{
    const auto circles = getAll();

    if (circles.empty()) return;

    double lon_min = circles[0].position.get<0>();
    double lat_min = circles[0].position.get<1>();
    double lon_max = lon_min;
    double lat_max = lat_min;

    for (const auto &c : circles)
    {
        lon_min = std::min(lon_min, c.position.get<0>());
        lon_max = std::max(lon_max, c.position.get<0>());
        lat_min = std::min(lat_min, c.position.get<1>());
        lat_max = std::max(lat_max, c.position.get<1>());
    }

    auto bottom_left_offset = vin_dir::apply(
        lon_min,
        lat_min,
        5,
        5 * M_PI_4,
        bg::srs::spheroid<double>()
    );

    auto top_right_offset = vin_dir::apply(
        lon_max,
        lat_max,
        5,
        M_PI_4,
        bg::srs::spheroid<double>()
    );

    lon_min = bottom_left_offset.lon2;
    lat_min = bottom_left_offset.lat2;

    lon_max = top_right_offset.lon2;
    lat_max = top_right_offset.lat2;

    double lon_center = 0.5 * (lon_min + lon_max);
    double lat_center = 0.5 * (lat_min + lat_max);

    double px_per_m_x = canvas.size().width /
        bg::distance(
            LatLon(lon_min, lat_center),
            LatLon(lon_max, lat_center)
        );

    double px_per_m_y = canvas.size().height /
        bg::distance(
            LatLon(lon_center, lat_min),
            LatLon(lon_center, lat_max)
        );

    double px_per_m = std::min(px_per_m_x, px_per_m_y);

    for (const auto &c : circles)
    {
        auto res = vin_inv::apply(
            lon_min,
            lat_min,
            c.position.get<0>(),
            c.position.get<1>(),
            bg::srs::spheroid<double>()
        );

        double ang = -res.azimuth + M_PI_2;

        double xm = res.distance * cos(ang);
        double ym = res.distance * sin(ang);

        cv::Point px(
            xm * px_per_m,
            canvas.size().height - ym * px_per_m
        );

        circle(
            canvas,
            px,
            0.5 * px_per_m,
            c.color == CircleColor::Brown
                ? cv::Scalar(69, 109, 146) :
            c.color == CircleColor::Gold
                ? cv::Scalar(69, 205, 255)
                : cv::Scalar(216, 244, 233),
            cv::FILLED
        );
    }
}
