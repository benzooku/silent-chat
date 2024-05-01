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

enum MessageType {
    Text,
    LongText,
    File,
    Status
};

enum SessionType {
    Request,
    Accept,
    Reject,
    Ongoing,
    Disabled
};

