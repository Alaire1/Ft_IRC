#include "Server.hpp"

int main()
{
    Server server;

    server.createServer();
    std::cout << "socket: " << server.getSocket() << std::endl;
    return (0);
}
