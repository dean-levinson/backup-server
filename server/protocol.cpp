#include <sstream>
#include "protocol.h"

using namespace std;

RequestParser::RequestParser(string data) {
    stringstream ss;
    ss << data;
    ss >> user_id;
    ss >> version;
    ss >> op;
    ss >> name_len;
    filename = string(name_len, ' ');

    // Compatible from version > C++03 when strings became contiguous.
    ss.readsome(&filename[0], name_len);
    ss >> size;
    
    payload = string(size, ' ');
    ss.readsome(&payload[0], name_len);
}; 

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
