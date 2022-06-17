#include <boost/asio.hpp>

#include "Hunter.h"

int main(int argc, char **argv)
{
    boost::asio::io_context io_context;

    Hunter hunter(io_context, {});

    io_context.run();

    return 0;
}
