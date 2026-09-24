#include "server.hpp"
#include "session.hpp"

#include <iostream>
#include <memory>

namespace fluxgate {

using boost::asio::ip::tcp;

Server::Server(boost::asio::io_context& io, uint16_t port)
    : acceptor_(io, tcp::endpoint(tcp::v4(), port)) {
    std::cout << "Fluxgate server listening on port " << port << "...\n";
    accept_next();
}

void Server::accept_next() {
    acceptor_.async_accept(
        [this](boost::system::error_code ec, tcp::socket socket) {
            if (!ec) {
                std::make_shared<Session>(std::move(socket))->start();
            }
            // Re-arm regardless of outcome — this is the line that keeps
            // the server accepting new clients forever.
            accept_next();
        });
}

} // namespace fluxgate