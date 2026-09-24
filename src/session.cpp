#include "session.hpp"
#include "protocol.hpp"

#include <cstring>
#include <iostream>
#include <stdexcept>

namespace fluxgate {

using boost::asio::ip::tcp;

Session::Session(tcp::socket socket, const Authenticator& authenticator) 
    : socket_(std::move(socket)), authenticator_(authenticator) {}

void Session::start() {
    std::cout << "Client connected: " << socket_.remote_endpoint() << " - awaiting token\n";
    read_header();
}

void Session::read_header() {
    auto self = shared_from_this();
    header_buf_.resize(4);

    boost::asio::async_read(
        socket_, boost::asio::buffer(header_buf_),
        [this, self](boost::system::error_code ec, std::size_t /*bytes*/) {
            if (ec) {
                std::cout << "Client disconnected.\n";
                return;
            }
            uint32_t length_be;
            std::memcpy(&length_be, header_buf_.data(), 4);
            uint32_t length = ntohl(length_be);
            read_body(length);
        });
}

void Session::read_body(uint32_t length) {
    auto self = shared_from_this();
    body_buf_.resize(length);

    boost::asio::async_read(
        socket_, boost::asio::buffer(body_buf_),
        [this, self](boost::system::error_code ec, std::size_t /*bytes*/) {
            if (ec) {
                std::cout << "Client disconnected.\n";
                return;
            }

            std::string received(body_buf_.begin(), body_buf_.end());

            if(!authenticated_) {

                if (authenticator_.verify(received)) {
                    authenticated_=true;
                    std::cout<<"Client authenticated.\n";
                    read_header();
                } else {
                    std::cout<<"Auth failed, closing connection.\n";
                }
                return;

            }

            std::cout<<"Recieved (" <<received.size()<<" bytes): " <<received << "\n";
            write_echo();
        });
}

void Session::write_echo() {
    auto self = shared_from_this();

    uint32_t length_be = htonl(static_cast<uint32_t>(body_buf_.size()));
    out_header_.resize(4);
    std::memcpy(out_header_.data(), &length_be, 4);

    std::vector<boost::asio::const_buffer> buffers{
        boost::asio::buffer(out_header_),
        boost::asio::buffer(body_buf_)};

    boost::asio::async_write(
        socket_, buffers,
        [this, self](boost::system::error_code ec, std::size_t /*bytes*/) {
            if (ec) {
                std::cout << "Write failed, client disconnected.\n";
                return;
            }
            read_header();
        });
}

} // namespace fluxgate