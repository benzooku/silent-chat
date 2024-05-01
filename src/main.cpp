#include "resources.h"

#include "p2p_client.hpp"
#include "session.hpp"


bool is_digits(const std::string &str)
{
    return str.find_first_not_of("0123456789") == std::string::npos;
}


int main(int argc, char* argv[])
{
    boost::asio::io_context io_context;
    int port = DEFAULT_PORT;

    std::thread thrContext;

    for(int i=1; i<argc; i++)
    {
        if(strcmp(argv[i], "--help") == 0 || strcmp(argv[i], "-h") == 0) {
            std::cerr << "Usage: p2p_client <host> /n Flags:\n--port (-p)\tDefines the Port to use (default: 48932)\n--help (-h)\tShows this message" << std::endl;
            return 1;
        }
        if(strcmp(argv[i], "--port") == 0 || strcmp(argv[i], "-p") == 0) {
            if(argv[i+1] != NULL && is_digits(argv[i+1])) {
                port = std::stoi(argv[i+1]);
            } else {
                std::cerr << "Invalid use of port flag" << std::endl;
                return 1;
            }
        }
    }


    try
    {
        P2P_Client p2p_client(io_context, port);

        thrContext = std::thread([&]() { io_context.run(); });
        while(true){
            std::string input;
            std::cin >> input;

            if(input == "help"){
                std::cout << "exit\nsend [receiver] [message]\nsendwport [receiver] [message] [port]\nsession [accept/reject]" << std::endl;
            }
            else if(input == "exit") {
                io_context.stop();
                break;
            }
            if(!p2p_client.session_manager.in_session()){
                if(input == "send"){
                    std::string receiver;
                    std::string message;
                    std::string port;
                    std::cin >> receiver;
                    std::cin >> message;

                    Message msg;
                    msg.header.id = MessageType::Text;
                    msg.header.session_type = SessionType::Disabled;
                    char buffer[1024];
                    strcpy(buffer, message.c_str());
                    msg.add(buffer);
                    p2p_client.send_message(receiver, DEFAULT_PORT, msg);
                }
                else if(input == "sendwport"){
                    std::string receiver;
                    std::string message;
                    std::string port;
                    std::cin >> receiver;
                    std::cin >> message;
                    std::cin >> port;

                    Message msg;
                    msg.header.id = MessageType::Text;
                    msg.header.session_type = SessionType::Disabled;
                    char buffer[1024];
                    strcpy(buffer, message.c_str());
                    msg.add(buffer);
                    p2p_client.send_message(receiver, std::stoi(port), msg);
                }
                else if(input == "session"){
                    std::string command;
                    std::cin >> command;
                    if(command == "accept") {
                    }
                    if(command == "reject") {

                    }
                }
            } else {
                if(input == "session"){

                } else {
                    Message msg;
                    msg.header.id = MessageType::Text;
                    msg.header.session_type = SessionType::Ongoing;
                    char buffer[1024];
                    strcpy(buffer, input.c_str());
                    msg.add(buffer);
                    p2p_client.send_message(p2p_client.session_manager.current_session().remote_endpoint.address().to_string(),
                                            p2p_client.session_manager.current_session().remote_endpoint.port(),
                                            msg);
                }

            }
        }
    }
    catch (std::exception& e)
    {
        std::cerr << e.what() << std::endl;
    }

    return 0;
}

