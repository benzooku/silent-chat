#include "resources.h"
#include "message.cpp"

#include "connection.cpp"

bool isDigits(const std::string& akStr)
{
    return akStr.find_first_not_of("0123456789") == std::string::npos;
}

enum ArgFlag {
    eHelp,
    ePort,
    eNone
};

ArgFlag GetFlag(const char* akArg)
{
  if (strcmp(akArg, "--help") == 0 || strcmp(akArg, "-h") == 0) {
    return eHelp;
  }
  if (strcmp(akArg, "--port") == 0 || strcmp(akArg, "-p") == 0) {
    return ePort;
  }
  return eNone;
}

auto ValidatePort(const char* akPort) -> bool
{
  return akPort != nullptr && isDigits(akPort);
}

auto main(int argc, char* argv[]) -> int
{
    boost::asio::io_context ioContext;
    std::thread thrContext;
    u_short port = DEFAULT_PORT;
    #pragma unroll 5
    for(int i=1; i<argc; i++)
    {
        if(GetFlag(argv[i]) == eHelp) {
            std::cerr << "Usage: p2p_client <host> /n Flags:\n--port (-p)\tDefines the Port to use (default: 48932)\n--help (-h)\tShows this message" << std::endl;
            return 1;
        }
        if(GetFlag(argv[i]) == ePort) {
            if(ValidatePort(argv[i+1])) {
                port = std::stoi(argv[i+1]);
            } else {
                std::cerr << "Invalid use of port flag" << std::endl;
                return 1;
            }
        }
    }


    try
    {
        networking::Connection connection(ioContext, port);

        thrContext = std::thread([&ioContext]() { ioContext.run(); });

        while(true){
            std::string input;
            std::cin >> input;

            if(input == "help"){
                std::cout << "exit\nsend [receiver] [message]\nsendwport [receiver] [message] [port]\nsession [accept/reject]\n";
            }
            else if(input == "exit") {
                ioContext.stop();
                break;
            }
            if(true){
                if(input == "send"){
                    std::string receiver;
                    std::string message;
                    std::cin >> receiver;
                    std::cin >> message;

                    networking::Message msg;
                    msg.header.id = MessageType::eText;
                    msg.header.session_type = SessionType::eDisabled;
                    std::array<char, MAX_MESSAGE_SIZE> buffer = {0};
                    std::copy(message.begin(), message.end(), buffer.data());
                    msg.add(buffer);
                    connection.sendMessage(receiver, DEFAULT_PORT, msg);
                }
                else if(input == "sendwport"){
                    std::string receiver;
                    std::string message;
                    std::string port;
                    std::cin >> receiver;
                    std::cin >> message;
                    std::cin >> port;

                    networking::Message msg;
                    msg.header.id = MessageType::eText;
                    msg.header.session_type = SessionType::eDisabled;
                    std::array<char, MAX_MESSAGE_SIZE> buffer = {0};
                    std::copy(message.begin(), message.end(), buffer.data());
                    msg.add(buffer);
                    connection.sendMessage(receiver, std::stoi(port), msg);
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
        std::cerr << e.what() << '\n';

    }

    return 0;
}

