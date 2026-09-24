#pragma once

#include "authenticator.hpp"
#include <boost/asio.hpp>

namespace fluxgate {

class Server {
public:
    Server(boost::asio::io_context& io, uint16_t port, const Authenticator& authenticator);

private:
    void accept_next();

    boost::asio::ip::tcp::acceptor acceptor_;
    const Authenticator& authenticator_;
};

}//namespace fluxgate