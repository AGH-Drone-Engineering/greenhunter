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

    Hunter hunter(
        io_context,
        {}
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
