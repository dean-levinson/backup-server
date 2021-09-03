#ifndef PROTOCOL_H_
#define PROTOCOL_H_

#include <string>

using namespace std;

class RequestParser {
    public:
    void init(iostream& stream);

    public:  
    uint32_t user_id;
    uint8_t version;
    uint8_t op;
    uint16_t name_len;
    string filename;
    uint32_t size;
    string payload;

    RequestParser(iostream& stream);
    RequestParser(string data);
};

class ResponseBuilder {
    public:  
    uint8_t version;
    uint16_t status;
    uint16_t name_len;
    string filename;
    uint32_t size;
    string payload;

    ResponseBuilder(uint8_t version, uint16_t status,
     string filename, string payload);
     string build();
     size_t response_size();
};

#endif /* PROTOCOL_H_ */
