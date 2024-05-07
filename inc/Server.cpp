#include "Server.hpp"
#include <cstddef>
//#include <string>
#include <iostream>

Server::Server(void) : _serSocketFd(-1)
{
	std::cout << "Server Default constructor called" << std::endl; 
}

Server::~Server(void) //
{
	std::cout << "Server Destructor called" << std::endl; 
}


void Server::serverInit()
{

}

void Server::serSocket()
{

}

void Server::acceptNewClient()
{

}

void Server::receiveNewData(int fd)
{

}

void Server::SignalHandler(int signum)
{

}

void Server::closeFds()
{

}

void Server::clearClients(int fd)
{
	for(size_t i = 0; i < fds.size(); i++)
	{
		if (fds[i].fd == fd)
		{
			fds.erase(fds.begin() + i);
			break;
		}
	}

	for(size_t i = 0; i < clients.size(); i++)
	{
		if (clients[i].getFd() == fd)
		{
			clients.erase(clients.begin() + i);
			break;
		}
	}
}
