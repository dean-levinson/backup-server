#include <iostream>
#include <thread>
using namespace std;


class ClientOperation {
    public:
        void operator()() {
            cout << "Do something" << endl;
        }
};

void run_server(string address, int port) {
    // bind to address:port
    
    /**
    while True:
        listen to incoming connections;
        when receiving a new connection, open a thread for it (For sure there are examples for accept -> open thread on the internet)
        handle the connection using the thread; 
    **/
}

int main() {
    string address;
    int port;
    cout << "Running server on " << address << ":" << port << endl;
    run_server(address, port);
    return 0;
}
