#include <sstream>
#include <iostream>
#include <vector>
#include <Windows.h>
#include "protocol.h"

using namespace std;

#define read_from_stream(stream, var) \
        stream.read(reinterpret_cast<char*>(&var), sizeof(var)); 

// Compatible from version > C++03 when strings became contiguous.
#define read_from_stream_to_string(stream, var, size) \
        var = string(size, '\0'); \
        stream.read(&var[0], size)

RequestParser::RequestParser(string data) {
    stringstream ss;
    ss << data;
    init(ss);
}; 

RequestParser::RequestParser(iostream& stream) {
    init(stream);
}; 

void RequestParser::init(iostream& stream) {
    read_from_stream(stream, user_id);
    read_from_stream(stream, version);
    read_from_stream(stream, op);
    read_from_stream(stream, name_len);
    read_from_stream_to_string(stream, filename, name_len);
    read_from_stream(stream, size);
    read_from_stream_to_string(stream, payload, size);
}
    
ResponseBuilder::ResponseBuilder(uint8_t version, uint16_t status, string filename, string payload):
 version(version), status(status), name_len(filename.size()), filename(filename),
  size(payload.size()), payload(payload) {};

size_t ResponseBuilder::response_size() {
    return sizeof(version) +
     sizeof(status) +
     sizeof(name_len) +
     filename.size() +
     sizeof(size) + 
     sizeof(payload);
}

string ResponseBuilder::build() {
    string data(response_size(), ' ');
    stringstream ss;
    ss << version;
    ss << status;
    ss << name_len;
    ss << filename;
    ss << size;
    ss << payload;
    ss >> data;

    return data;
}
