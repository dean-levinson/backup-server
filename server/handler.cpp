#include <boost/asio.hpp>
#include <iostream>
#include "handler.h"
#include "protocol.h"

using boost::asio::ip::tcp;

#define SERVER_VERSION (1)

Handler::Handler(tcp::iostream& stream): request(stream) {};

string Handler::handle_request() {
    string response;
    switch (request.op) {
        case BACKUP_FILE:
            response = backup_file(); break;
        // case RESTORE_FILE:
        //     response = restore_file(); break;
        // case REMOVE_FILE:
        //     response = remove_file(); break;
        // case LIST_FILES:
        //     response = list_files();
    }
    return response;
}

string Handler::backup_file() {
    string s("Backup succeffuly!");
    ResponseBuilder builder(SERVER_VERSION, BACKUP_SUCCESS, s);   
    return builder.get_response();
}