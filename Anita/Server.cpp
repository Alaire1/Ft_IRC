#include "Server.hpp"
#include <signal.h>
#include <iostream>
#include <cstring>

Server::Server()
{
    _isRunning = false;
    _socket = 0;
    _servInfo = NULL;
}

Server::~Server()
{
    freeaddrinfo(_servInfo);
}
void Server::createServer()
{
    _isRunning = false; // server is not running yet

    struct addrinfo hints;
    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_UNSPEC; // use IPv4 or IPv6
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE; // fill in my IP for me

    int status = getaddrinfo(NULL, "3490", &hints, &_servInfo);
    if (status != 0) {
        std::cerr << "getaddrinfo error: " << gai_strerror(status) << std::endl;
        exit(1);
    }

    createSocket();
    handleSignals();
}
int Server::createSocket()
{
    _socket = socket(_servInfo->ai_family, _servInfo->ai_socktype, _servInfo->ai_protocol);
    if (_socket == -1)
    {
        std::cerr << "Error: socket creation failed" << std::endl;
        exit(1);
    }
    return (0);
}

void Server::handleSignals()
{
    signal(SIGINT, signalHandler);
    signal(SIGTERM, signalHandler);
}

int Server::getSocket() const
{
    return (_socket);
}

void Server::signalHandler(int signum)
{
    std::cout  <<"Signal received" << signum  << std::endl;
}