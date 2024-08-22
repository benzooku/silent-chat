#pragma once
#include "resources.h"
#include "message.cpp"


namespace networking {

using boost::asio::ip::udp;

class Session {
public:
    udp::endpoint remoteEndpoint;
    std::string remoteName;
    bool isOpen = false;
};

class SessionManager{
public:
    void handle_session_type(udp::endpoint aRemoteEndpoint, Message aMsg){
        bool isActiveSession = false;
        int index = -1;
        for(int i = 0; i < MAX_CONNECTIONS; i++){
            if(mSessions[i].isOpen){
                if(mSessions[i].remoteEndpoint == aRemoteEndpoint){
                    isActiveSession = true;
                    index = i;
                }
            }
        }

        if(!isActiveSession){
            switch (aMsg.header.session_type)
            {
            case SessionType::eRequest:
                std::cout << aRemoteEndpoint << " requested a session [session accept/reject]" << std::endl;
                break;
            case SessionType::eAccept:
                break;
            case SessionType::eReject:
                break;
            case SessionType::eOngoing:
                break;
            case SessionType::eDisabled:
                break;

            default:
                break;
            }
        } else {
            switch (aMsg.header.session_type)
            {
            case SessionType::eRequest:
                std::cout << aRemoteEndpoint << " requested a session [session accept/reject]" << std::endl;
                break;
            case SessionType::eAccept:
                break;
            case SessionType::eReject:
                break;
            case SessionType::eOngoing:
                break;
            case SessionType::eDisabled:
                break;

            default:
                break;
            }
        }
    }
public:
    Session currentSession(){
        return mSessions[mCurrentSessionId];
    }

private:
    std::array<Session, MAX_CONNECTIONS> mSessions;
    int mCurrentSessionId = 0;
    int mSessionCount = -1;
    bool mInSession = false;

};

}
