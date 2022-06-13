#include <iostream>
#include <boost/asio.hpp>
#include <boost/bind/bind.hpp>
#include <boost/thread.hpp>

#include "Scout.h"

int main(int argc, char **argv)
{
    boost::asio::io_context io_context;
    boost::asio::executor_work_guard<boost::asio::io_context::executor_type>
        work = boost::asio::make_work_guard(io_context);
    boost::thread io_thread(boost::bind(&boost::asio::io_context::run, &io_context));

    Scout scout(io_context, {});

    try
    {
        scout.run();
    }
    catch (std::exception &ex)
    {
        std::cerr << "Error: " << ex.what() << std::endl;
    }

    io_context.stop();
    io_thread.join();

    return 0;
}
