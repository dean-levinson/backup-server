#ifndef SERVER_H_
#define SERVER_H_

#include <cstdlib>
#include <iostream>
#include <thread>
#include <utility>
#include <boost/asio.hpp>

using namespace boost::asio::ip;
using boost::asio::ip::tcp;

/**
 * Accepts a client connection from io_context, and open thread to handle it.
 * 
 * @param io_context - the io_context of the boost application.
 * @param address - the address which the server should bind to.
 * @param port - the TCP port which the server should bind to.
 */
void run_server(boost::asio::io_context& io_context, address_v4 address, unsigned short port);

#endif /* SERVER_H_ */