#include <boost/asio.hpp>
#include <string>
#include "protocol.h"

using boost::asio::ip::tcp;

class Handler {
    private:
    RequestParser request;
    string backup_file();
    string restore_file();
    string remove_file();
    string list_files();
    public:
    Handler(tcp::iostream& stream);
    string handle_request();
};