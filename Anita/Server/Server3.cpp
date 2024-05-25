#include "Server.hpp"
#include <cstdlib>
#include <iostream>
#include <cstring>
#include <fcntl.h>
#include <unistd.h>
#include <signal.h>
#include <poll.h>
#include <vector>
#include <stdexcept>
#include <arpa/inet.h>
#include <netdb.h>
#include <sys/socket.h>

#define BUFFER_SIZE 1024

Server::Server() : _socket(-1),  _port(6667), _password("1234"), _servInfo(nullptr) {}

Server::Server(int port, std::string password) :  _socket(-1), _port(port), _password(std::move(password)), _servInfo(nullptr) {}

bool Server::_signal = false;

Server::~Server() 
{
    freeaddrinfo(_servInfo);
}

void Server::createServer() {
    initializeHints();
    handleSignals();
    createAndSetSocket();
    startServer();
}

void Server::handle_sigstop(int sig) {
    if (sig == SIGTSTP) {
        std::cout << "SIGTSTP received. Stopping server..." << std::endl;
        _signal = true;
    }
}

void Server::handleSignals() {
    signal(SIGTSTP, handle_sigstop);
}

int Server::pollFds() {
    int ready = poll(_fds.data(), _fds.size(), -1);
    if (ready == -1) {
        if(errno == EINTR)
            return 0;
        errorPoll(errno);
        return -1;
    }
    return ready;
}

void Server::handleNewConnection() {
    std::cout << "Handle new connection" << std::endl;
    Client cli;
    struct sockaddr_in clientAddr;
    socklen_t addrLen = sizeof(clientAddr);
    int clientSocket = accept(_socket, (struct sockaddr*)&clientAddr, &addrLen);
    if (clientSocket == -1) {
        std::cerr << "Error accepting connection: " << strerror(errno) << std::endl;
        return;
    }
    if (fcntl(clientSocket, F_SETFL, O_NONBLOCK) < 0) {
        std::cerr << "Error setting client socket to non-blocking: " << strerror(errno) << std::endl;
        close(clientSocket);
        return;
    }
    addFd(clientSocket, POLLIN);
    cli.setFd(clientSocket);
    _clients.push_back(cli);
    std::cout << "Client socket added to _clients!" << std::endl;
}

void Server::addFd(int newFd, short events) {
    struct pollfd newPoll;
    newPoll.fd = newFd;
    newPoll.events = events;
    newPoll.revents = 0;
    _fds.push_back(newPoll);
    std::cout << "Client socket [" << newFd << "] added to _fds!" << std::endl;
}

void Server::handleExistingConnection(int fd) {
    char buffer[BUFFER_SIZE];
    memset(buffer, 0, sizeof(buffer));
    int bytes = recv(fd, buffer, sizeof(buffer), 0);
    if (bytes < 0) {
        std::cerr << "Error receiving data: " << strerror(errno) << std::endl;
        close(fd);
        fd = -1;
    } else if (bytes == 0) {
        std::cerr << "Client disconnected" << std::endl;
        clearClients(fd);
        close(fd);
        fd = -1;
    } else {
        std::cout << "[Client " << fd << "] Received data: " << std::string(buffer, bytes) << std::endl;
    }
}

void Server::startServer() {
    std::cout << "Server < " << _socket << " > waiting for connection... " << std::endl;
    while (!_signal) {
        if (poll(_fds.data(), _fds.size(), -1) == -1 && !_signal) {
            std::cerr << "Error polling: " << strerror(errno) << std::endl;
            break;
        }
        for (size_t i = 0; i < _fds.size(); i++) {
            if (_fds[i].revents & POLLIN) {
                if(_fds[i].fd == _socket) {
                    handleNewConnection();
                } else {
                    handleExistingConnection(_fds[i].fd);
                }
            }
        }
    }
    closeFds();
}

void Server::initializeHints() {
    struct addrinfo hints;
    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE;
    std::string str = std::to_string(_port);
    const char *cstr = str.c_str();
    int status = getaddrinfo(NULL, cstr, &hints, &_servInfo);
    if (status != 0) {
        std::cerr << "Error in getaddrinfo: " << gai_strerror(status) << std::endl;
        exit(EXIT_FAILURE);
    }
}

void Server::createAndSetSocket() {
    _socket = socket(AF_INET, SOCK_STREAM, 0);
    if (_socket == -1) {
        std::cerr << "Error creating socket: " << strerror(errno) << std::endl;
        exit(EXIT_FAILURE);
    }
    int reuse = 1;
    if (setsockopt(_socket, SOL_SOCKET, SO_REUSEADDR, &reuse, sizeof(reuse)) < 0) {
        std::cerr << "Error setting socket options: " << strerror(errno) << std::endl;
        close(_socket);
        exit(EXIT_FAILURE);
    }
    if (fcntl(_socket, F_SETFL, O_NONBLOCK) == -1) {
        std::cerr << "Error setting non-blocking mode: " << strerror(errno) << std::endl;
        close(_socket);
        exit(EXIT_FAILURE);
    }
    struct sockaddr_in add;
    add.sin_family = AF_INET;
    add.sin_addr.s_addr = INADDR_ANY;
    add.sin_port = htons(_port);
    if (bind(_socket, (struct sockaddr *)&add, sizeof(add)) == -1) {
        std::cerr << "Error binding socket: " << strerror(errno) << std::endl;
        close(_socket);
        exit(EXIT_FAILURE);
    }
    if (listen(_socket, SOMAXCONN) == -1) {
        std::cerr << "Error listening on socket: " << strerror(errno) << std::endl;
        close(_socket);
        exit(EXIT_FAILURE);
    }
    struct pollfd newPoll;
    newPoll.fd = _socket;
    newPoll.events = POLLIN;
    newPoll.revents = 0;
    _fds.push_back(newPoll);
}

void Server::closeFds()
{
	for(size_t i = 0; i < _fds.size(); i++)
	{ //-> close all the clients
		if (_fds[i].fd  == -1)
		{
			std::cout << "Client <" << _clients[i].getFd() << "> Disconnected" << std::endl;
			close(_fds[i].fd);
		}
	}
	for(size_t i = 0; i < _clients.size(); i++)
	{ //-> close all the clients
		if (_clients[i].getFd()  == -1)
		{
			std::cout << "Client <" << _clients[i].getFd() << "> Disconnected" << std::endl;
			close(_clients[i].getFd());
		}
	}
	
}

void Server::clearClients(int fd) //-> clear the clients
{
	for(std::vector<struct pollfd>::iterator it = _fds.begin(); it != _fds.end();)//-> remove the client from the pollfd
	{
		if (it->fd == fd)
		{
			_fds.erase(it);
			//break;
		}
		else
			it++;
	}
	for (std::vector<struct Client>::iterator it = _clients.begin(); it != _clients.end();)
	{
		if (it->getFd() == fd)
		{
			_clients.erase(it);
			//break;
		}
		else
			it++;
	}
}

int Server::getSocket() const
{
    return (_socket);
}

void Server::printPassword()
{
    std::cout << "Password: " << _password << std::endl;
}
