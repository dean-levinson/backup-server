#include <boost/asio.hpp>
#include <server.h>

using namespace boost::asio::ip;
using boost::asio::ip::tcp;



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