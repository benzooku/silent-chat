#pragma once

#include <boost/asio.hpp>
#include <iostream>
#include <array>
#include <ctime>
#include <functional>
#include <memory>
#include <string>
#include <thread>

#define MAX_CONNECTIONS 20
#define DEFAULT_PORT 48932
#define MAX_MESSAGE_SIZE 1024

enum MessageType {
    eText,
    eLongText,
    eFile,
    eStatus
};

enum SessionType {
    eRequest,
    eAccept,
    eReject,
    eOngoing,
    eDisabled
};

