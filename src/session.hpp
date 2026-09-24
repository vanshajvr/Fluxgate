#pragma once

#include <boost/asio.hpp>
#include <memory>
#include <vector>

namespace fluxgate {

// One Session per connected client. Manages its own socket and buffers,
// independent of every other client.
//
// enable_shared_from_this lets a Session hand out a shared_ptr to ITSELF
// (via shared_from_this()) — that's how we keep it alive for as long as
// there's a pending async operation on it, even after the function that
// created it has returned.
class Session : public std::enable_shared_from_this<Session> {
public:
    explicit Session(boost::asio::ip::tcp::socket socket);

    void start();

private:
    void read_header();
    void read_body(uint32_t length);
    void write_echo();

    boost::asio::ip::tcp::socket socket_;
    std::vector<char> header_buf_;
    std::vector<char> body_buf_;
    std::vector<char> out_header_;
};

} // namespace fluxgate