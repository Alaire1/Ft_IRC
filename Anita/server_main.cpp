#include "Server.hpp"
#include "HelperFunctions.hpp"
int main(int ac, char **av)
{
    if (ac != 3)
    {
        std::cerr << "Usage: ./server [port] [host]" << std::endl;
        return (1);
    }
    checkIfPortIsValid(av[1]);

    Server server(atoi(av[1]), av[2]); 

    server.createServer();
    std::cout << "socket: " << server.getSocket() << std::endl;
    server.printPassword();
    return (0);
}
