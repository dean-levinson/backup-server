#ifndef PROTOCOL_H_
#define PROTOCOL_H_

#include <string>

using namespace std;

enum OP_CODE {
    BACKUP_FILE = 100,
    RESTORE_FILE = 200,
    REMOVE_FILE = 201,
    LIST_FILES = 202
};

enum STATUS_CODE {
    RESTORE_SUCCESS = 210,
    LIST_FILES_SUCCESS = 211,
    BACKUP_OR_REMOVE_SUCCESS = 212,
    FILE_NOT_FOUND = 1001,
    NO_FILES = 1002,
    GENERAL_FAILURE = 1003
};

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
    string response; 
    uint8_t version;
    uint16_t status;
    uint16_t name_len;
    string filename;
    uint32_t size;
    string payload;

    ResponseBuilder(uint8_t version, uint16_t status,string filename, string payload);   
    ResponseBuilder(uint8_t version, uint16_t status, string filename);
    ResponseBuilder(uint8_t version, uint16_t status);
    string get_response();
};

#endif /* PROTOCOL_H_ */
