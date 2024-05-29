#include "Server.hpp"
#include <exception>

int main(int ac, char **av)
{
    if (ac != 3)
    {
        std::cerr << "Usage: ./server [port] [password]" << std::endl;//Currently is the client set to port 4444
        return (1);
    }
    Server ourServer(atoi(av[1]), av[2]); 
		try
		{
			signal(SIGINT, Server::signalHandler);
			signal(SIGQUIT, Server::signalHandler);
			ourServer.initServer();
		}
		catch(const std::exception& e)
		{
			//ourServer.closeFds();
			std::cerr << e.what() << std::endl;
		}
		std::cout << "Server destroyed" << std::endl;
    return (0);
}
