#pragma once
#include "resources.h"
#include "message.hpp"
#include "session.hpp"

using boost::asio::ip::udp;

namespace networking {


class handler {
public:

    static void incoming(const udp::endpoint &remote_endpoint, Message msg){
        //session_manager.handle_session_type(remote_endpoint, msg);

        std::cout << "me <- " << remote_endpoint << ":\t";

        std::array<char, 1024> temp;
        switch (msg.header.id)
        {
        case MessageType::Text:
            msg.read(temp);
            std::cout << temp.data() << std::endl;
            break;

        default:
            break;
        }
    }

    static void outgoing(const udp::endpoint &remote_endpoint, Message msg){
        std::array<char, 1024> temp;
        msg.read(temp);
        std::cout << "me -> " << remote_endpoint << ":\t" << temp.data() << std::endl;
    }


};

}
