#include "Client.hpp"

Client::Client(void) : _fd(-1), _nick(""), _user(""), _isRegistered(false), _hasPassword(false) {}

Client::~Client(void) {}

int Client::getFd() const {return _fd;}

std::string Client::getNick() const {return _nick;}

std::string Client::getUser() const {return _user;}

bool Client::getIsRegistered() const {return _isRegistered;}

bool Client::getHasPassword() const {return _hasPassword;}

void Client::setFd(int fd) {_fd = fd;}

void Client::setNickName(std::string& nick) {_nick= nick;}

void Client::setUserName(std::string& user) {_user= user;}

void Client::setIsRegistered(bool status) {_isRegistered= status;}

void Client::setHasPassword(bool status) {_hasPassword= status;}

void Client::sendMessage(const std::string& message)
{
	const char* cmessage = message.c_str();

	send(_fd, cmessage, message.size(), 0);
}

bool Client::operator==(const Client& other) const
{
    return this->_nick== other._nick; // Assuming id is a member variable of Client
}


