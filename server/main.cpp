#include <cstdlib>
#include <iostream>
#include <thread>
#include <utility>
#include <boost/asio.hpp>

using namespace boost::asio::ip;
using boost::asio::ip::tcp;


const int max_length = 1024;

void session(tcp::socket sock)
{
  try
  {
    for (;;)
    {
      char data[max_length];

      boost::system::error_code error;
      size_t length = sock.read_some(boost::asio::buffer(data), error);
      if (error == boost::asio::error::eof)
        break; // Connection closed cleanly by peer.
      else if (error)
        throw boost::system::system_error(error); // Some other error.

      boost::asio::write(sock, boost::asio::buffer(data, length));
    }
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
      tcp::socket sock(a.accept());
      tcp::endpoint remote_ep = sock.remote_endpoint();
      std::cout << "Starting session with (" << remote_ep.address() << ", " << remote_ep.port() << ")" << std::endl; 
      std::thread(session, std::move(sock)).detach();
    }
}


int main(int argc, char* argv[])
{
  try
  {
    if (argc != 3)
    {
      std::cerr << "Usage: <ip address> <port>\n";
      return 1;
    }
    
    address_v4 server_address = make_address_v4(argv[1]);
    unsigned short server_port = std::atoi(argv[2]);
    boost::asio::io_context io_context;
 
   run_server(io_context, server_address, server_port);
  }
  catch (std::exception& e)
  {
    std::cerr << "Exception: " << e.what() << "\n";
  }

  return 0;
}