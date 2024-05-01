#pragma once
#include "resources.h"
#include "p2p_client.hpp"



namespace networking {
using boost::asio::ip::udp;

class session {
public:
    udp::endpoint remote_endpoint;
    std::string remote_name;
    bool is_open;
};

class session_manager{
public:
    void handle_session_type(udp::endpoint remote_endpoint, Message msg){
        bool isActiveSession = false;
        int index = -1;
        for(int i = 0; i < MAX_CONNECTIONS; i++){
            if(sessions[i].is_open){
                if(sessions[i].remote_endpoint == remote_endpoint){
                    isActiveSession = true;
                    index = i;
                }
            }
        }

        if(!isActiveSession){
            switch (msg.header.session_type)
            {
            case SessionType::Request:
                std::cout << remote_endpoint << " requested a session [session accept/reject]" << std::endl;
                break;
            case SessionType::Accept:
                break;
            case SessionType::Reject:
                break;
            case SessionType::Ongoing:
                break;
            case SessionType::Disabled:
                break;

            default:
                break;
            }
        } else {
            switch (msg.header.session_type)
            {
            case SessionType::Request:
                std::cout << remote_endpoint << " requested a session [session accept/reject]" << std::endl;
                break;
            case SessionType::Accept:
                break;
            case SessionType::Reject:
                break;
            case SessionType::Ongoing:
                break;
            case SessionType::Disabled:
                break;

            default:
                break;
            }
        }
    }
public:
    bool in_session(){
        return b_in_session;
    }
public:
    Session current_session(){
        return sessions[current_session_id];
    }

private:
    Session sessions[MAX_CONNECTIONS]; // maybe vector or other in future
    int current_session_id = 0;
    int session_count = -1;
    bool b_in_session = false;

};

}
