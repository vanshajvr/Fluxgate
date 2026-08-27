#include <boost/asio.hpp>
#include <cstdint>
#include <cstring>
#include <iostream>
#include <vector>

using boost::asio::ip::tcp;

constexpr uint16_t SERVER_PORT=9000;

std::vector<char> read_message(tcp::socket& socket){

    uint32_t length_be=0;
    boost::asio::read(socket, boost::asio::buffer(&length_be, sizeof(length_be)));

    uint32_t length=ntohl(length_be);

    std::vector<char> payload(length);
    if(length>0){
        boost::asio::read(socket, boost::asio::buffer(payload.data(), length));
    }

    return payload;
}

void send_message(tcp::socket& socket, const std::vector<char>& payload){
    uint32_t length_be=htonl(static_cast<uint32_t>(payload.size()));

    boost::asio::write(socket, boost::asio::buffer(&length_be, sizeof(length_be)));
    if (!payload.empty()){
        boost::asio::write(socket, boost::asio::buffer(payload));
    }
}

int main(){
    try{
        boost::asio::io_context io;

        tcp::acceptor acceptor(io,tcp::endpoint(tcp::v4(), SERVER_PORT));
        std::cout<<"DAQ server listening on port " << SERVER_PORT << "...\n";

        tcp::socket socket(io);
        acceptor.accept(socket);
        std::cout <<"Client connected: " << socket.remote_endpoint() << "\n";

        while(true){
            std::vector<char>msg=read_message(socket);

            std::string as_text(msg.begin(), msg.end());
            std::cout<< "Recieved (" << msg.size() << " bytes): " <<as_text<<"\n";

            send_message(socket, msg);
        }
    } catch(const std:: exception& e){

        std::cout <<"Connection closed: "<<e.what()<< "\n";
    }

    return 0;
}