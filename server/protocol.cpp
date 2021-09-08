#include <sstream>
#include <iostream>
#include <vector>
#include <Windows.h>
#include "protocol.h"

using std::string;

#define read_from_stream(stream, var) \
        stream.read(reinterpret_cast<char*>(&var), sizeof(var))

// Compatible from version > C++03 when strings became contiguous.
#define read_from_stream_to_string(stream, var, size) \
        var = string(size, '\0'); \
        stream.read(&var[0], size)

#define write_to_stream(stream, var) \
        stream.write(reinterpret_cast<char*>(&var), sizeof(var))

#define write_string_to_stream(stream, var) \
        stream.write(&var[0], var.size())

RequestParser::RequestParser(string data) {
    std::stringstream ss;
    ss << data;
    init(ss);
}; 

RequestParser::RequestParser(std::iostream& stream) {
    init(stream);
}; 

void RequestParser::init(std::iostream& stream) {
    read_from_stream(stream, user_id);
    read_from_stream(stream, version);
    read_from_stream(stream, op);
    if (op == BACKUP_FILE || op == RESTORE_FILE || op == REMOVE_FILE) {
        read_from_stream(stream, name_len);
        read_from_stream_to_string(stream, filename, name_len);
    }
    if (op == BACKUP_FILE) {
        read_from_stream(stream, size);
        read_from_stream_to_string(stream, payload, size);
    }
}
    
ResponseBuilder::ResponseBuilder(uint8_t version, uint16_t status, string filename, string payload):
 version(version), status(status), name_len(filename.size()), filename(filename),
  size(payload.size()), payload(payload) {
     std::stringstream ss;
     write_to_stream(ss, version);
     write_to_stream(ss, status);
     write_to_stream(ss, name_len);
     write_string_to_stream(ss, filename);
     write_to_stream(ss, size);
     write_string_to_stream(ss, payload);
     response = string(std::istreambuf_iterator<char>(ss), {});
  };

ResponseBuilder::ResponseBuilder(uint8_t version, uint16_t status, string filename):
 version(version), status(status), name_len(filename.size()), filename(filename) {
     std::stringstream ss;
     write_to_stream(ss, version);
     write_to_stream(ss, status);
     write_to_stream(ss, name_len);
     write_string_to_stream(ss, filename);
     response = string(std::istreambuf_iterator<char>(ss), {});
 };

ResponseBuilder::ResponseBuilder(uint8_t version, uint16_t status):
 version(version), status(status) {
     std::stringstream ss;
     write_to_stream(ss, version);
     write_to_stream(ss, status);
     response = string(std::istreambuf_iterator<char>(ss), {});
 };

string ResponseBuilder::get_response() {
    return response;
}
