#include "resources.h"

#include "connection.hpp"
#include "session.hpp"

auto is_digits(const std::string &str) -> bool
{
    return str.find_first_not_of("0123456789") == std::string::npos;
}

enum arg_flag {
    help,
    port,
    none
};

arg_flag get_flag(const char* arg){
    if(strcmp(arg, "--help") == 0 || strcmp(arg, "-h") == 0){
        return help;
    } else if(strcmp(arg, "--port") == 0 || strcmp(arg, "-p") == 0){
        return port;
    } else {
        return none;
    }
}

bool char_valid_port(const char* port){
    if(port != nullptr && is_digits(port)) {
        return true;
    }
    return false;
}


int main(int argc, char* argv[])
{
    boost::asio::io_context io_context;
    std::thread thrContext;
    u_short port = DEFAULT_PORT;
    for(int i=1; i<argc; i++)
    {
        if(get_flag(argv[i]) == help) {
            std::cerr << "Usage: p2p_client <host> /n Flags:\n--port (-p)\tDefines the Port to use (default: 48932)\n--help (-h)\tShows this message" << std::endl;
            return 1;
        }
        if(get_flag(argv[i]) == port) {
            if(char_valid_port(argv[i+1])) {
                port = std::stoi(argv[i+1]);
            } else {
                std::cerr << "Invalid use of port flag" << std::endl;
                return 1;
            }
        }
    }


    try
    {
        networking::connection connection(io_context, port);

        thrContext = std::thread([&io_context]() { io_context.run(); });
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
            if(true){
                if(input == "send"){
                    std::string receiver;
                    std::string message;
                    std::string port;
                    std::cin >> receiver;
                    std::cin >> message;

                    Message msg;
                    msg.header.id = MessageType::Text;
                    msg.header.session_type = SessionType::Disabled;
                    std::array<char, 1024> buffer = {0};
                    std::copy(message.begin(), message.end(), buffer.data());
                    msg.add(buffer);
                    connection.send_message(receiver, DEFAULT_PORT, msg);
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
                    std::array<char, 1024> buffer = {0};
                    std::copy(message.begin(), message.end(), buffer.data());
                    msg.add(buffer);
                    connection.send_message(receiver, std::stoi(port), msg);
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

