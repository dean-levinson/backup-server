#include <cstdlib>
#include <iostream>
#include <thread>
#include <utility>
#include <string>
#include <boost/asio.hpp>
#include "protocol.h"
#include "handler.h"

using namespace boost::asio::ip;
using boost::asio::ip::tcp;


const int max_length = 1024;

void session(tcp::iostream stream)
{
  try
  {
    /** 
     * The handler gets the socket as a stream and reads from it on the fly. The alternative was to read the whole stream
     * into a buffer and then handle the request. The main drawback of the alternative is that the passing file will be save
     * in the memory of the server before handling. This can be avoid by reading directly from the socket.
     */ 
    Handler handler(stream);
    string response = handler.handle_request();

    stream << response;
  }
  catch (std::exception& e)
  {
    std::cerr << "Exception in thread: " << e.what() << "\n";
  }
}

void run_server(boost::asio::io_context& io_context, address_v4 address, unsigned short port) {
    std::cout << "Running server on: " << address << ":" << port << std::endl; 
    tcp::acceptor a(io_context, tcp::endpoint(address, port));
    
    for (;;)
    {
      tcp::iostream stream;
      a.accept(stream.socket());
      tcp::endpoint remote_ep = stream.socket().remote_endpoint();

      std::cout << "Starting session with (" << remote_ep.address() << ", " << remote_ep.port() << ")" << std::endl; 

      // Because of using move, the thread will think it gets a rvalue parameter,
      //  and therefor the socket's destructor will be called at the end of the scope.
      std::thread(session, std::move(stream)).detach();
    }
}
