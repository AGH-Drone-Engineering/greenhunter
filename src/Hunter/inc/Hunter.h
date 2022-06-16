#ifndef GREENHUNTER_HUNTER_H
#define GREENHUNTER_HUNTER_H

#include <boost/asio.hpp>

class Hunter
{
public:
    struct Params
    {

    };

    Hunter(boost::asio::io_context &io_context,
           const Params &params);

    void run();

private:

};

#endif
