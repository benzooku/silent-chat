#pragma once
#include "resources.h"
#include "message.hpp"
#include "session.hpp"

namespace networking {
using boost::asio::ip::udp;

class Connection {

public:
    Connection(boost::asio::io_context& io_context, int port)
    : socket_rec(io_context, udp::endpoint(udp::v6(), port)),
    resolver_(io_context), socket_send(io_context),
    io_context_(io_context){
        source_port = port;
        socket_send.open(udp::v6());
        default_dest_port = DEFAULT_PORT;

        receive_header();
    }

    ~Connection(){
        socket_send.close();
        socket_rec.close();
    }

public:
    void send_message(std::string receiver, int port, const Message& msg){
        udp::endpoint receiver_endpoint =
            *resolver_.resolve(udp::v6(), receiver, std::to_string(port)).begin();
        boost::asio::post(io_context_,
                [this, msg, receiver_endpoint]()
                {
                        send_header(receiver_endpoint, msg);
                });
    }
    void send_header(udp::endpoint receiver_endpoint, const Message& msg){
            socket_send.async_send_to(boost::asio::buffer(&msg.header, sizeof(Message_Header)), receiver_endpoint,
                [this, msg, receiver_endpoint](std::error_code ec, std::size_t length){
                    if (!ec) {
                        if(msg.size() > 0){
                            send_body(receiver_endpoint, msg);
                        }
                    }else {
                        std::cerr << ec.message() << std::endl;
                    }
                });
    }
    void send_body(udp::endpoint receiver_endpoint, const Message& msg){
        socket_send.async_send_to(boost::asio::buffer(msg.body.data(), msg.size()), receiver_endpoint,
                [this, msg, receiver_endpoint](std::error_code ec, std::size_t length){
                    if (!ec) {
                        handle_send(msg, receiver_endpoint);
                    } else {
                        std::cerr << ec.message() << std::endl;
                    }
                });
    }
    void receive_header(){
        socket_rec.async_receive_from(
            boost::asio::buffer(&recv_msg_temp.header, sizeof(Message_Header)), remote_endpoint_,
            [this](std::error_code ec, std::size_t length){
                if (!ec){
                    if(recv_msg_temp.header.size > 0){
                        recv_msg_temp.body.resize(recv_msg_temp.header.size);
                        receive_body();
                    } else{
                        handle_receive(remote_endpoint_, recv_msg_temp);
                    }
                } else {
                    std::cerr << ec.message() << std::endl;
                }
            });
    }

    void receive_body(){
        socket_rec.async_receive_from(
            boost::asio::buffer(recv_msg_temp.body.data(), recv_msg_temp.body.size()), remote_endpoint_,
            [this](std::error_code ec, std::size_t length){
                if (!ec){
                    handle_receive(remote_endpoint_, recv_msg_temp);
                } else {
                    std::cerr << ec.message() << std::endl;
                }
            });
    }

protected:
    void handle_receive(udp::endpoint remote_endpoint, Message msg)
    {

        session_manager.handle_session_type(remote_endpoint, msg);

        std::cout << "me <- " << remote_endpoint.address().to_string() << ":\t";

        std::string str;
        char temp[1024];
        switch (msg.header.id)
        {
        case MessageType::Text:
            msg.read(temp);
            std::cout << temp << std::endl;
            break;

        default:
            break;
        }

        receive_header();
    }

private:
    void handle_send(const Message& msg, udp::endpoint receiver_endpoint)
    {
        std::cout << "me -> " << receiver_endpoint << ":\t" << std::endl;
    }

private:
    boost::asio::io_context& io_context_;
    udp::socket socket_rec;
    udp::socket socket_send;
    udp::resolver resolver_;
    udp::endpoint remote_endpoint_;
    Message recv_msg_temp;

    int source_port;

    int default_dest_port;

};

}
