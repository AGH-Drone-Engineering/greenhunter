#include <boost/asio.hpp>

#include "Scout.h"

int main(int argc, char **argv)
{
    boost::asio::io_context io_context;

    Scout scout(io_context, {});

    std::thread io_thread([&] {
        io_context.run();
    });

    scout.run();

    io_context.stop();
    io_thread.join();

    return 0;
}
