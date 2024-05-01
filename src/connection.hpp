#pragma once
#include "resources.h"
#include "message.hpp"
#include "session.hpp"
#include "handler.hpp"

namespace networking {
using boost::asio::ip::udp;

class connection {

public:
    connection(boost::asio::io_context& io_context, u_short port)
    : io_context_(io_context),
    socket_rec(io_context, udp::endpoint(udp::v6(), port)), socket_send(io_context),
    resolver_(io_context){
        socket_send.open(udp::v6());

        receive_header();
    }

    ~connection(){
        socket_rec.close();
        socket_send.close();
    }

    void send_message(const std::string &receiver, const int port, const Message& msg){
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
                [this, msg, receiver_endpoint](std::error_code ec, std::size_t){
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
                [this, msg, receiver_endpoint](std::error_code ec, std::size_t){
                    if (!ec) {
                        handle_send(receiver_endpoint, msg);
                    } else {
                        std::cerr << ec.message() << std::endl;
                    }
                });
    }
    void receive_header(){
        socket_rec.async_receive_from(
            boost::asio::buffer(&recv_msg_temp.header, sizeof(Message_Header)), remote_endpoint_,
            [this](std::error_code ec, std::size_t){
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
            [this](std::error_code ec, std::size_t){
                if (!ec){
                    handle_receive(remote_endpoint_, recv_msg_temp);
                } else {
                    std::cerr << ec.message() << std::endl;
                }
            });
    }

private:
    void handle_receive(const udp::endpoint &remote_endpoint, const Message &msg)
    {
        handler::incoming(remote_endpoint, msg);
        receive_header();
    }

    void handle_send(const udp::endpoint &remote_endpoint, const Message &msg )
    {
        handler::outgoing(remote_endpoint, msg);
    }

    boost::asio::io_context& io_context_;
    udp::socket socket_rec;
    udp::socket socket_send;
    udp::resolver resolver_;
    udp::endpoint remote_endpoint_;
    Message recv_msg_temp;

};

}
