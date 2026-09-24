#pragma once

#include <boost/asio.hpp>

namespace fluxgate {

class Server {
public:
    Server(boost::asio::io_context& io, uint16_t port);

private:
    void accept_next();

    boost::asio::ip::tcp::acceptor acceptor_;
};

}//namespace fluxgate