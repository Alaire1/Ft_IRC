#include "Client.hpp"
#include <string>
#include <iostream>

Client::Client(void) //: _init(some)
{
	std::cout << "Client Default constructor called" << std::endl; 
}

Client::~Client(void) //
{
	std::cout << "Client Destructor called" << std::endl; 
}

int Client::getFd() const
{
	return _fd;
}

void Client::setFd(int fd) 
{
	_fd = fd;
}

void Client::setIpAdd(std::string ipadd)
{
	_ipAdd = ipadd;
}


