#include "Server.hpp"
#include <signal.h>
#include <iostream>
#include <cstring>
#include <map>

Server::Server() : _isRunning(false), _socket(0), _servInfo(NULL)
{
   
}

Server::~Server()
{
    freeaddrinfo(_servInfo);
}

void printGetaddrinfoError(int status) {
    std::map<int, std::string> errorMessages = {
        {EAI_AGAIN, "Temporary failure in name resolution"},
        {EAI_BADFLAGS, "Invalid value for ai_flags"},
        {EAI_FAIL, "Non-recoverable failure in name resolution"},
        {EAI_FAMILY, "The ai_family does not support the specified ai_socktype"},
        {EAI_MEMORY, "Memory allocation failure"},
        {EAI_NONAME, "Neither nodename nor servname provided, or not known"},
        {EAI_SERVICE, "The ai_socktype is not supported for the specified ai_family or the specified service is not supported for the specified ai_socktype"},
        {EAI_SOCKTYPE, "The specified ai_socktype is not supported"},
        {EAI_OVERFLOW, "Argument buffer overflow"}
    };
    std::cerr << "getaddrinfo error: ";
    if (errorMessages.count(status) > 0) {
        std::cerr << errorMessages[status];
    } else {
        std::cerr << "Unknown error";
    }
    std::cerr << std::endl;
    exit(1);
}

void Server::createServer()
{
    struct addrinfo hints;
    memset(&hints, 0, sizeof(hints)); // hints are better to be local variable since they are just used once 
    hints.ai_family = AF_UNSPEC; // use IPv4 or IPv6, AF_INET or AF_INET6 , AF_UNSPEC is the most flexible
    hints.ai_socktype = SOCK_STREAM; // use TCP, which guarantees delivery
    hints.ai_flags = AI_PASSIVE; // fill in my IP for me

    int status = getaddrinfo(NULL, "6667", &hints, &_servInfo); // later we can change the port to be a parameter
    if (status != 0) {
        printGetaddrinfoError(status);
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