#include "Client.hpp"
#include <netdb.h>

Client::Client(void) {}

Client::Client(const std::string& ipAdd, const std::string& port) : _ipAdd(ipAdd), _port(port), _fd(-1) {}

Client::~Client(void) {}

int Client::getFd() const {return _fd;}

void Client::setFd(int fd) {_fd = fd;}

void Client::setIpAdd(std::string ipadd) {_ipAdd = ipadd;}

bool Client::connectServer()
{
	struct addrinfo hints, *res, *p;
	memset(&hints, 0, sizeof(hints));
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;

	int status = getaddrinfo(_ipAdd.c_str(), _port.c_str(), &hints, &res);

	if (status != 0)
	{
		std::cerr << "getaddrinfo: " << gai_strerror(status) << std::endl;
	}
	for (p = res; p != NULL; p = p->ai_next)
	{
		_fd = socket(p->ai_family, p->ai_socktype, p->ai_protocol);
		if (_fd == -1)
		{
			perror("socket");
			continue;
		}
		if(connect(_fd, p->ai_addr, p->ai_addrlen) == -1)
		{
			close(_fd);
			perror("connect");
			continue;
		}
		break;
	}
	freeaddrinfo(res);
	if(p == NULL)
	{
		std::cerr << "Failed to connect" << std::endl;
		return false;
	}
	std::cout << "Connect to the server" << std::endl;
	return true;
}




