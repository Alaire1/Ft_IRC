#include "Server.hpp"


Server::Server() : _isRunning(false), _socket(0), _servInfo(NULL), _port(6667), _password("12345")
{
   
}

Server::Server(int port, std::string password) : _isRunning(false), _socket(0), _servInfo(NULL)
{
    _port = port;
    _password = password;
}

Server::~Server()
{
    freeaddrinfo(_servInfo);
}


void Server::createServer()
{
    initializeHints();
    createSocket();
    handleSignals();

}


void Server::initializeHints()
{
    struct addrinfo hints; // freed by itself because it is a local variable
    memset(&hints, 0, sizeof(hints)); // hints are better to be local variable since they are just used once 
    hints.ai_family = AF_UNSPEC; // use IPv4 or IPv6, AF_INET or AF_INET6 , AF_UNSPEC is the most flexible
    hints.ai_socktype = SOCK_STREAM; // use TCP, which guarantees delivery
    hints.ai_flags = AI_PASSIVE; // fill in my IP for me
    int status = getaddrinfo(NULL, "6667", &hints, &_servInfo); // later we can change the port to be a parameter
    if (status != 0) {
        errorPrintGetaddrinfo(status);
        exit(1);
    }
}

int Server::createSocket() // may split into smaller fumnctions
{
    _socket = socket(_servInfo->ai_family, _servInfo->ai_socktype, _servInfo->ai_protocol);
    if (_socket == -1)
    {
        errorSocketCreation(errno);
        exit(1);
    }
    // Set the socket to be reusable, so we can bind it again even if it is in TIME_WAIT state
    int reuse = 1; 
    int result = setsockopt(_socket, SOL_SOCKET, SO_REUSEADDR, &reuse, sizeof(reuse));
    if (result == -1) {
       errorSetsockopt(errno); 
        close(_socket); // Close the socket on error
        return (1);
    }
    // Set the socket to non-blocking mode
    int flags = fcntl(_socket, F_GETFL, 0);
    if (flags == -1)
    {
        errorFcntl(errno);
        exit(1);
    }
    flags |= O_NONBLOCK; 
    if (fcntl(_socket, F_SETFL, flags) == -1)
    {
        errorFcntl(errno);
        exit(1);
    }
    // Bind the socket to the port
    if (bind(_socket, _servInfo->ai_addr, _servInfo->ai_addrlen) == -1)
    {
        errorSocketBinding(errno);
        exit(1);
    }
    // Start listening on the socket // BACKLOG is the number of connections that can be waiting while the process is handling a particular connection
    if (listen(_socket, BACKLOG) == -1) {
        errorListen(errno);
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
 
void Server::printPassword()
{
    std::cout << "Password: " << _password << std::endl;
}
