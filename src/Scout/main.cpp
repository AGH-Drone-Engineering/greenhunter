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

    Scout::Params params;
    params.drone.camera.fov_h = 60. * boost::geometry::math::d2r<double>();
    params.drone.camera.fov_v = 36. * boost::geometry::math::d2r<double>();
    params.drone.camera.frame_width = 1280;
    params.drone.camera.frame_height = 720;

    Scout scout(
        io_context,
        0,
        params
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
