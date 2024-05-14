#include "Client.hpp"
#include <string>


Client::Client(void) {}

Client::~Client(void) {}

int Client::getFd() const {return _fd;}

void Client::setFd(int fd) {_fd = fd;}

void Client::setIpAdd(std::string ipadd) {_ipAdd = ipadd;}


