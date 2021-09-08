#ifndef PROTOCOL_H_
#define PROTOCOL_H_

#include <ostream>
#include <string>

using std::string;

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
    /**
     * Initialize to construction from a stream.
     * 
     * @param stream - A stream that contains the client's request.
     */
    void init(std::iostream& stream);

    public:  
    uint32_t user_id;
    uint8_t version;
    uint8_t op;
    uint16_t name_len;
    string filename;
    uint32_t size;
    string payload;

    /**
     * Construct a new Request Parser object
     * 
     * @param stream - A stream that contains the client's request.
     */
    RequestParser(std::iostream& stream);

    /**
     * Construct a new Request Parser object
     * 
     * @param data - A string that contains the client's request.
     */
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

    /**
     * Construct a new Response Builder object, with the relevant params.
     */
    ResponseBuilder(uint8_t version, uint16_t status,string filename, string payload);   

    /**
     * Construct a new Response Builder object, with the relevant params.
     */
    ResponseBuilder(uint8_t version, uint16_t status, string filename);

    /**
     * Construct a new Response Builder object, with the relevant params.
     */
    ResponseBuilder(uint8_t version, uint16_t status);

    /**
     * Get the response.
     * 
     * @return string - A string that contains the response.
     */
    string get_response();
};

#endif /* PROTOCOL_H_ */
