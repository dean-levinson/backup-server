#ifndef HANDLER_H_
#define HANDLER_H_

#include <boost/asio.hpp>
#include <string>
#include "backup.h"
#include "protocol.h"

using boost::asio::ip::tcp;

/**
 * Handles specific request from the client from point to point - 
 * Parses the request, handles the request, and finally generates a response.
 * 
 */
class Handler {
    private:
    RequestParser request;
    Backup backup;
    string backup_file();
    string restore_file();
    string remove_file();
    string list_files();
    string generate_filename();
    
    public:
    /**
     * Construct a new Handler object.
     * 
     * @param stream - The stream that contains the client's request.
     */
    Handler(tcp::iostream& stream);

    /**
     * Handles the request and returns the response.
     * 
     * @return string - A string that contains the response. 
     */
    string handle_request();
};

#endif /* HANDLER_H_ */