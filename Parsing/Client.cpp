#include "Client.hpp"


Client::Client(void) : isRegistered(false), hasPassword(false) {_ipAdd = "127.0.0.1";}

Client::~Client(void) {}

int Client::getFd() const {return _fd;}

void Client::setFd(int fd) {_fd = fd;}

void Client::setIpAdd(std::string ipadd) {_ipAdd = ipadd;}

void Client::setNickname(std::string& nickName) {_nick = nickName;}

void Client::setUsername(std::string& userName) {_user = userName;}

std::string Client::getNick() const {return _nick;}

std::string Client::getUser() const {return _user;}

void Client::sendMessage(const std::string& message)
{
	const char* cmessage = message.c_str();

	send(_fd, cmessage, message.size(), 0);
}



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


