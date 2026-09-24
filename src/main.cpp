#include "protocol.hpp"
#include "server.hpp"

#include <boost/asio.hpp>
#include <iostream>

int main() {
    try{
        boost::asio::io_context io;
        fluxgate::Server server(io,fluxgate::SERVER_PORT);
        io.run();
    } catch(const std::exception& e) {
        std::cerr <<"Fatal error: " <<e.what() << "\n";
    }
    return 0;
}