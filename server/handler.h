#include <boost/asio.hpp>
#include "protocol.h"

using boost::asio::ip::tcp;

class Handler {
    private:
    RequestParser request;
    public:
    Handler(tcp::iostream& stream);
    string handle_request();
};