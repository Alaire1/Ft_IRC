#include "Client.hpp"


Client::Client(void) {_ipAdd = "127.0.0.1";}

Client::~Client(void) {}

int Client::getFd() const {return _fd;}

void Client::setFd(int fd) {_fd = fd;}

void Client::setIpAdd(std::string ipadd) {_ipAdd = ipadd;}

void Client::setNickname(std::string& nickName) {_nick = nickName;}

std::string Client::getNick() {return _nick;}
//bool Client::connectServer()
//{
//	struct addrinfo hints, *res, *p;
//	memset(&hints, 0, sizeof(hints));
//	hints.ai_family = AF_UNSPEC;
//	hints.ai_socktype = SOCK_STREAM;
//
//	int status = getaddrinfo(_ipAdd.c_str(), _port.c_str(), &hints, &res);
//
//	if (status != 0)
//	{
//		std::cerr << "getaddrinfo: " << gai_strerror(status) << std::endl;
//	}
//	for (p = res; p != NULL; p = p->ai_next)
//	{
//		_fd = socket(p->ai_family, p->ai_socktype, p->ai_protocol);
//		if (_fd == -1)
//		{
//			perror("socket");
//			continue;
//		}
//		if(connect(_fd, p->ai_addr, p->ai_addrlen) == -1)
//		{
//			close(_fd);
//			perror("connect");
//			continue;
//		}
//		std::cout << "Successfully connected to the server" << std::endl;
//		break;
//	}
//	freeaddrinfo(res);
//	if(p == NULL)
//	{
//		std::cerr << "Failed to connect" << std::endl;
//		return false;
//	}
//	std::cout << "Connected to the server" << std::endl;
//	return true;
//}
//
//void Client::sendMessage(const std::string& message)
//{
//	if (send(_fd, message.c_str(), message.length(), 0) == -1)
//		perror("send");
//	std::cout << "Message sent" << std::endl;
//
//}
//
//void Client::receiveMessage()
//{
//	char buffer[1024];
//	while (true)
//	{
//		memset(buffer, 0, sizeof(buffer));
//		int bytes_received = recv(_fd, buffer, sizeof(buffer) - 1, 0);
//		if (bytes_received > 0)
//			std::cout << "Server: " << buffer << std::endl;
//		else if (bytes_received == 0)
//		{
//			std::cout << "Connection closed by server." << std::endl;
//			break;
//		}
//		else
//		{
//			perror("recv");
//			break;
//		}
//	}
//}


