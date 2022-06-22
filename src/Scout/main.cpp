#include <iostream>
#include <boost/asio.hpp>
#include <boost/thread/scoped_thread.hpp>

#include "Scout.h"

namespace b = boost;
namespace ba = b::asio;

int main(int argc, char **argv)
{
    ba::io_context io_context;

    ba::executor_work_guard<ba::io_context::executor_type>
        work = ba::make_work_guard(io_context);

    b::strict_scoped_thread<> io_thread(b::thread(
        [&] { io_context.run(); }
    ));

    cv::Size frame_size(1280, 720);
    double fov_h = 60. * boost::geometry::math::d2r<double>();

    Scout scout(
        io_context,
        0,
        {
            .drone = {
                .camera = {
                    .fov_h = fov_h,
                    .fov_v = 2. * atan(tan(fov_h * 0.5) / frame_size.aspectRatio()),
                    .frame_width = frame_size.width,
                    .frame_height = frame_size.height,
                },
            }
        }
    );

    try
    {
        scout.run();
    }
    catch (std::exception &ex)
    {
        std::cerr << "Error: " << ex.what() << std::endl;
    }

    work.reset();
    io_context.stop();

    return 0;
}
