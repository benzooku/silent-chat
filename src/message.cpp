#pragma once
#include "resources.h"


namespace networking {


struct Message_Header {
    MessageType id{};
    SessionType session_type{};
    uint32_t size = 0;
};

class Message {
public:
    Message_Header header{};
    std::vector<uint8_t> body;


    size_t size() const
    {
        return body.size();
    }

    template<typename DataType>
    void add(const DataType& akData) {
        static_assert(std::is_standard_layout<DataType>::value, "Data is too complex to be pulled from vector");

        size_t s = body.size();
        body.resize(s + sizeof(DataType));
        std::memcpy(body.data() + s, &akData, sizeof(DataType));

        header.size = body.size();
    }

    template<typename DataType>
    void read(DataType& aData) {
        static_assert(std::is_standard_layout<DataType>::value, "Data is too complex to be pulled from vector");

        size_t s = body.size() - sizeof(DataType);

        std::memcpy(&aData, body.data() + s, sizeof(DataType));

        body.resize(s);
        header.size = size();
    }
};

}
