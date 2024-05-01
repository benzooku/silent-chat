#pragma once
#include "resources.h"
#include "message.cpp"
#include "session.cpp"

using boost::asio::ip::udp;

namespace networking {


class Handler {
public:

    static void processIncoming(const udp::endpoint &akRemoteEndpoint, Message aMsg){
        //session_manager.handle_session_type(akRemoteEndpoint, msg);

        std::cout << "me <- " << akRemoteEndpoint << ":\t";

        std::array<char, MAX_MESSAGE_SIZE> buffer;
        switch (aMsg.header.id)
        {
        case MessageType::eText:
            aMsg.read(buffer);
            std::cout << buffer.data() << '\n';
            break;

        default:
            break;
        }
    }

    static void processOutgoing(const udp::endpoint &akRemoteEndpoint, Message aMsg){
        std::array<char, MAX_MESSAGE_SIZE> buffer;
        aMsg.read(buffer);
        std::cout << "me -> " << akRemoteEndpoint << ":\t" << buffer.data() << '\n';
    }


};

}
