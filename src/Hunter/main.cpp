#include <iostream>
#include <boost/asio.hpp>
#include <boost/thread.hpp>
#include <boost/thread/scoped_thread.hpp>

#include "Hunter.h"

int main(int argc, char **argv)
{
    boost::asio::io_context io_context;

    boost::asio::executor_work_guard<boost::asio::io_context::executor_type>
            work = boost::asio::make_work_guard(io_context);

    boost::strict_scoped_thread<> io_thread(
        boost::thread(
            boost::bind(
                &boost::asio::io_context::run,
                &io_context
            )
        )
    );

    Hunter hunter(io_context, {});

    try
    {
        hunter.run();
    }
    catch (std::exception &ex)
    {
        std::cerr << "Error: " << ex.what() << std::endl;
    }

    work.reset();

    return 0;
}
