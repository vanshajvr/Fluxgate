#include <boost/asio.hpp>
#include <cstdint>
#include <cstring>
#include <iostream>
#include <memory>
#include <vector>

using boost::asio::ip::tcp;

constexpr uint16_t SERVER_PORT=9000;

// One session per connection, manages its own socket and buffers, independent of other clients.
// enable_shared_from_this lets a Session hand out a shared_ptr to ITSELF.
// (via shared_from_this()), so that it can keep itself alive while async operations are in progress.

class Session: public std::enable_shared_from_this<Session>{
    public:
        explicit Session(tcp::socket socket): socket_(std::move(socket)) {}

        void start(){
            std::cout<< "Client connected: "<< socket_.remote_endpoint()<< "\n";
            read_header();
        }
    
    private:
        void read_header(){
            //'self' keeps this session running until lambda below runs
            //without it, session will be destroyed while asio still holds pointer to it,
            //and the callback would fire on freed memory.
            auto self=shared_from_this();
            header_buf_.resize(4);

            boost::asio::async_read(
                socket_, boost::asio::buffer(header_buf_),
                [this,self](boost::system::error_code ec, std::size_t /*bytes*/)
                {
                    if(ec){
                        std::cout<<"Client disconnected.\n";
                        return; //chain stops here; session is destroyed once
                        //'self' (and any other shared_ptr copies) go away.
                    }

                    uint32_t length_be;
                    std::memcpy(&length_be, header_buf_.data(),4);
                    uint32_t length=ntohl(length_be);
                    read_body(length);
                });
        }

        void read_body(uint32_t length){
            auto self=shared_from_this();
            body_buf_.resize(length);

            boost::asio::async_read(
                socket_, boost::asio::buffer(body_buf_),
                [this,self](boost::system::error_code ec, std::size_t /*bytes*/)
                {
                    if(ec){
                        std::cout<<"Client disconnected./n";
                        return;
                    }
                    std::string as_text(body_buf_.begin(), body_buf_.end());
                    std::cout<<"Received (" << body_buf_.size()<< " bytes): "<< as_text << "\n";
                    write_echo();
                });
        }

        void write_echo(){
            auto self=shared_from_this();

            uint32_t length_be=htonl(static_cast<uint32_t>(body_buf_.size()));
            out_header_.resize(4);
            std::memcpy(out_header_.data(), &length_be, 4);

            //two seperate buffers (header,body) sent as one async_write call
            //asio will write both in order without us manually chaining 2 writes.

            std::vector<boost::asio::const_buffer> buffers{
                boost::asio::buffer(out_header_),
                boost::asio::buffer(body_buf_)};

            boost::asio::async_write(
                socket_, buffers,
                [this,self](boost::system::error_code ec, std::size_t /*bytes*/)
                {
                    if(ec){
                        std::cout<<"Write failed, client disconnected.\n";
                        return;
                    }

                    read_header();// loop: go read the NEXT message from this client
                });
        }

        tcp::socket socket_;
        std::vector<char> header_buf_;
        std::vector<char> body_buf_;
        std::vector<char> out_header_;
};

//owns the acceptor and keeps accept-chain running.
class Server{
public:
    Server(boost::asio::io_context& io, uint16_t port): acceptor_(io, tcp::endpoint(tcp::v4(), port))
    {
        std::cout<< "Fluxgate server listening on port "<<port<< "...\n";
        accept_next();
    }
private:
    void accept_next(){
        acceptor_.async_accept(
            [this](boost::system::error_code ec, tcp::socket socket)
            {
                if(!ec){
                    std::make_shared<Session>(std::move(socket))->start();
                }
                //re-arm regardless of outcome
                //this is the line that keeps server accepting new clients forever.
                accept_next();
            });
    }

    tcp::acceptor acceptor_;
};

int main(){
    try{
        boost::asio::io_context io;
        Server server(io, SERVER_PORT);
        io.run();// event loop starts here; blocks, dispatching callbacks forever
    }
    catch(const std::exception& e){
        std::cerr<<"Fatal error: "<<e.what()<<"\n";
    }

    return 0;
}


