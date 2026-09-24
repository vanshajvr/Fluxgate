#include "protocol.hpp"
#include "server.hpp"
#include "token_list_authenticator.hpp"

#include <boost/asio.hpp>
#include <iostream>

int main() {
    try{
        boost::asio::io_context io;

        fluxgate::TokenListAuthenticator authenticator{"vansh-dev-token", "test-client-token"};
        fluxgate::Server server(io,fluxgate::SERVER_PORT, authenticator);
        io.run();
    } catch(const std::exception& e) {
        std::cerr <<"Fatal error: " <<e.what() << "\n";
    }
    return 0;
}