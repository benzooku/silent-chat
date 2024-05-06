#pragma once
#include "resources.h"
#include "message.cpp"
#include "session.cpp"
#include "handler.cpp"

#include <queue>

namespace networking {
using boost::asio::ip::udp;

class Connection {

public:
    Connection(boost::asio::io_context& aIOContext, const u_short akPort, std::queue<Message>& aQueue)
    : mIOContext(aIOContext),
    mSocketReceive(aIOContext, udp::endpoint(udp::v6(), akPort)), mSocketSend(aIOContext),
    mResolver(aIOContext), mMessageQueue(aQueue){
        mSocketSend.open(udp::v6());

        receiveHeader();
    }

    virtual ~Connection(){
        try {
            mSocketReceive.close();
            mSocketSend.close();
        }
        catch (std::exception& e) {
            std::cerr << e.what() << std::endl;
        }
    }

    void sendMessage(const std::string &akReceiver, const int akPort, const Message& akMsg){
        const udp::endpoint kReceiverEndpoint =
            *mResolver.resolve(udp::v6(), akReceiver, std::to_string(akPort)).begin();
        boost::asio::post(mIOContext,
                [this, akMsg, kReceiverEndpoint]()
                {
                        sendHeader(kReceiverEndpoint, akMsg);
                });
    }
    void sendHeader(const udp::endpoint &akReceiverEndpoint, const Message& akMsg){
            mSocketSend.async_send_to(boost::asio::buffer(&akMsg.header, sizeof(Message_Header)), akReceiverEndpoint,
                [this, akMsg, akReceiverEndpoint](std::error_code error, std::size_t){
                    if (!error) {
                        if(akMsg.size() > 0){
                            sendBody(akReceiverEndpoint, akMsg);
                        }
                    }else {
                        std::cerr << error.message() << "\n";
                    }
                });
    }
    void sendBody(const udp::endpoint &akReceiverEndpoint, const Message& akMsg){
        mSocketSend.async_send_to(boost::asio::buffer(akMsg.body.data(), akMsg.size()), akReceiverEndpoint,
                [this, akMsg, akReceiverEndpoint](std::error_code error, std::size_t){
                    if (!error) {
                        handleSend(akReceiverEndpoint, akMsg);
                    } else {
                        std::cerr << error.message() << '\n';
                    }
                });
    }
    void receiveHeader(){
        mSocketReceive.async_receive_from(
            boost::asio::buffer(&mMsgBuffer.header, sizeof(Message_Header)), mRemoteEndpoint,
            [this](std::error_code error, std::size_t){
                if (!error){
                    if(mMsgBuffer.header.size > 0){
                        mMsgBuffer.body.resize(mMsgBuffer.header.size);
                        receiveBody();
                    } else{
                        handleReceive(mRemoteEndpoint, mMsgBuffer);
                    }
                } else {
                    std::cerr << error.message() << '\n';
                }
            });
    }

    void receiveBody(){
        mSocketReceive.async_receive_from(
            boost::asio::buffer(mMsgBuffer.body.data(), mMsgBuffer.body.size()), mRemoteEndpoint,
            [this](std::error_code error, std::size_t){
                if (!error){
                    handleReceive(mRemoteEndpoint, mMsgBuffer);
                } else {
                    std::cerr << error.message() << '\n';
                }
            });
    }

private:
    void handleReceive(const udp::endpoint &remote_endpoint, const Message &msg)
    {
        mMessageQueue.push(msg);
        Handler::processIncoming(remote_endpoint, msg);
        receiveHeader();
    }

    void handleSend(const udp::endpoint &remote_endpoint, const Message &msg )
    {
        Handler::processOutgoing(remote_endpoint, msg);
    }

    boost::asio::io_context& mIOContext;
    udp::socket mSocketReceive;
    udp::socket mSocketSend;
    udp::resolver mResolver;
    udp::endpoint mRemoteEndpoint;
    Message mMsgBuffer;


    std::queue<Message> &mMessageQueue;

};

} // namespace networking
