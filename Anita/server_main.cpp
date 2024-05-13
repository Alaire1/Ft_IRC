#include "Server.hpp"
#include "HelperFunctions.hpp"

#include <cstring>
int main(int ac, char **av)
{
    if (ac != 3)
    {
        std::cerr << "Usage: ./server [port] [password]" << std::endl;
        return (1);
    }
    checkIfPortIsValid(av[1]);
    checkPassword(av[2]);
    Server ourServer(atoi(av[1]), av[2]); 

    ourServer.createServer();
    std::cout << "socket: " << ourServer.getSocket() << std::endl;
    ourServer.printPassword();
    return (0);
}
