#ifndef HANDLER_H_
#define HANDLER_H_

#include <boost/asio.hpp>
#include <string>
#include "backup.h"
#include "protocol.h"

using boost::asio::ip::tcp;

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
    Handler(tcp::iostream& stream);
    string handle_request();
};

#endif /* HANDLER_H_ */