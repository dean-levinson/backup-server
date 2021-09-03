#include <boost/asio.hpp>
#include <iostream>
#include "handler.h"
#include "protocol.h"

using boost::asio::ip::tcp;

Handler::Handler(tcp::iostream& stream): request(stream) {};

string Handler::handle_request() {
    std::cout << "Filename: " << request.filename << std::endl;
    return "Hello world";
}
