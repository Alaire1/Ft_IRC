#include "../inc/Server.hpp"
#include <exception>

bool validPort(int port);


int main(int ac, char **av)
{

    if (ac != 3)
    {
        std::cerr << "Usage: ./server [port] [password]" << std::endl;
        return (1);
    }

		if(validPort(atoi(av[1])))
		{
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
				std::cerr << MAGENTA << e.what() << RESET << std::endl;
			}
			std::cout << RED << "Server destroyed!!!" << RESET << std::endl;
		}
}

bool validPort(int port)
{
	return ((port > 6660 && port < 6669) || port == 7000);
}
