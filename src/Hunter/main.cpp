#include <iostream>
#include <boost/asio.hpp>
#include <boost/thread/scoped_thread.hpp>

#include "Hunter.h"

namespace b = boost;
namespace ba = b::asio;

int main(int argc, char **argv)
{
    boost::asio::io_context io_context;

    ba::executor_work_guard<ba::io_context::executor_type>
        work = ba::make_work_guard(io_context);

    b::strict_scoped_thread<> io_thread(b::thread(
        [&] { io_context.run(); }
    ));

    cv::Size frame_size(1280, 720);
    double fov_h = 60. * boost::geometry::math::d2r<double>();
    CameraParams camera_params = {
        fov_h,
        2. * atan(tan(fov_h * 0.5) / frame_size.aspectRatio()),
        frame_size.width,
        frame_size.height
    };

    Hunter hunter(
        io_context,
        0,
        {
            .camera = camera_params,
        }
    );

    try
    {
        hunter.run();
    }
    catch (std::exception &ex)
    {
        std::cerr << "Error: " << ex.what() << std::endl;
    }

    work.reset();
    io_context.stop();

    return 0;
}
