#include "Server.hpp"


Server::Server() : _isRunning(false), _socket(0), _servInfo(NULL), _port(6667), _password("1234")
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
    handleSignals();
    createAndSetSocket();
    //startServer();
}
void Server::handleSignals()
{
    signal(SIGINT, signalHandler);
    signal(SIGTERM, signalHandler);
}

// void Sever::startServer()
// {
//     _isRunning = true;
//     while (_isRunning)
//     {
//         // accept connections
        
//         // handle connections
//         // close connections
//     }
// }
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

void Server::createSocket()
{
    _socket = socket(_servInfo->ai_family, _servInfo->ai_socktype, _servInfo->ai_protocol);
    if (_socket == -1)
    {
        errorSocketCreation(errno);
        close(_socket); // Close the socket on error
        exit(1);
    }
}
void Server::setSocketReusable()
{
    int reuse = 1; 
    int result = setsockopt(_socket, SOL_SOCKET, SO_REUSEADDR, &reuse, sizeof(reuse));
    if (result == -1) {
       errorSetsockopt(errno); 
        close(_socket); // Close the socket on error
        exit (1); // not sure if i should return 1 or exit(1) 
    }
}
void Server::nonBlockingSocket()
{
   int flags = fcntl(_socket, F_GETFL, 0); // Get the current flags on the socket
    if (flags == -1)
    {
        errorFcntl(errno);
        close(_socket); // Close the socket on error
        exit(1);
    }
    flags |= O_NONBLOCK; 
    if (fcntl(_socket, F_SETFL, flags) == -1)
    {
        errorFcntl(errno);
        close(_socket); // Close the socket on error
        exit(1);
    }
}

void Server::bindSocket()
{
    if (bind(_socket, _servInfo->ai_addr, _servInfo->ai_addrlen) == -1)
    {
        errorSocketBinding(errno);
        close(_socket); // Close the socket on error
        exit(1);
    }
}
void Server::listenSocket()
{
    // BACKLOG is the number of connections that can be waiting while the process is handling a particular connection
    if (listen(_socket, BACKLOG) == -1) {
        errorListen(errno);
        close(_socket); // Close the socket on error
        exit(1);
    }
}
int Server::createAndSetSocket() // may split into smaller fumnctions
{
    createSocket();
    // Set the socket to be reusable, so we can bind it again even if it is in TIME_WAIT state
    setSocketReusable();
    // Set the socket to non-blocking mode
    nonBlockingSocket();
    // Bind the socket to the port
    bindSocket();
    // Start listening on the socket 
    listenSocket();
    return (0);
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
