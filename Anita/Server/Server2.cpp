#include "Server.hpp"
#include <asm-generic/socket.h>
#include <cstring>
#include <netdb.h>
#include <string>
#include <strings.h>
#include <sys/socket.h>


Server::Server() : _socket(-1), _servInfo(NULL), _port(6667), _password("1234")
{

}

Server::Server(int port, std::string password) :  _socket(-1), _servInfo(NULL)
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
	startServer();
}
void Server::handleSignals()
{
    signal(SIGINT, signalHandler);
    signal(SIGTERM, signalHandler);
}


 int Server::pollFds()
 {
     //int timeout = 1000; // 1 second timeout // random value
     int ready = poll(&_fds[0], _fds.size(), -1);
     if (ready == -1)
     {
			 if(errno == EINTR)
				 return 0;
			 errorPoll(errno);
			 return (-1);
     }
		 return ready;
 }

 void Server::handleNewConnection()
 {
	std::cout << "Handle new connection" << std::endl;
    Client cli;
    struct sockaddr_in clientAddr;
    socklen_t addrLen = sizeof(clientAddr);
	std::cout << "server socket fd: " << _socket << std::endl;
    int clientSocket = accept(_socket, (struct sockaddr*)&clientAddr, &addrLen);
	std::cout << "New fd: " << clientSocket << std::endl;
	if (clientSocket == -1)
	{
		if (errno == EAGAIN || errno == EWOULDBLOCK)
		{
			// No pending connections, not an error
			return;
		}
		else
		{
			// errorAccept(errno);
			std::cout << "error accept " << strerror(errno) << std::endl;
			return;
		}
	}
	if (fcntl(clientSocket, F_SETFL, O_NONBLOCK) < 0)
	{
		std::cerr << "Error setting client socket to non-blocking"  << std::endl;
		return;
	}
	else
		std::cout << "Accept success!!" << std::endl;
	std::cout << "Client socket: " << clientSocket << std::endl;
	addFd(clientSocket, POLLIN);
	cli.setFd(clientSocket);					  //-> set the client file descriptor
	cli.setIpAdd(inet_ntoa(clientAddr.sin_addr)); //-> convert the ip address to string and set it
	_clients.push_back(cli);					  //-> add the client to the vector of clients
	std::cout << "client fd added to vector!!" << std::endl;
	//printclientfds(_clients);
	std::cout << _clients.size() << std::endl;
 }

 void Server::addFd(int newFd, short events)
 {
	std::cout << "in addFD Client socket: " << newFd << std::endl;
	struct pollfd newPoll;
	newPoll.fd = newFd;		 //-> set the client file descriptor
	newPoll.events = events; //
	newPoll.revents = 0;	 //-> add the client to the vector of clients
	_fds.push_back(newPoll); //-> add the client socket to the pollfd
	printfds(_fds);
 }

//void Server::handleExistingConnection(int fd) {
//    std::cout << "inside existing Connection" << std::endl;
//    char buffer[1024];
//    memset(buffer, 0, sizeof(buffer)); // Clear the buffer
//
//    while (true) {
//        int bytes = recv(fd, buffer, sizeof(buffer), 0);
//
//        if (bytes == -1) {
//            std::cout << "Bytes is " << bytes << std::endl;
//            std::cerr << "recv() error: " << strerror(errno) << std::endl;
//
//            if (errno == EINTR) {
//                // Interrupted by a signal, try again
//                continue;
//            } else if (errno == EAGAIN || errno == EWOULDBLOCK) {
//                // No data available for non-blocking socket
//                break;
//            } else {
//                // Other errors
//                std::cout << "Error on recv and negative bytes" << std::endl;
//                close(fd);
//
//                fd = -1; // Mark for removal
//                break;
//            }
//        } else if (bytes == 0) {
//            // Client disconnected
//            std::cout << "Client disconnected" << std::endl;
//            close(fd);
//            fd = -1; // Mark for removal
//            break;
//        } else {
//            // Handle the received data
//            std::cout << "Here we should be handling incoming data..." << std::endl;
//            std::cout << "Received data: " << std::string(buffer, bytes) << std::endl;
//        }
//    }
//}

void Server::handleExistingConnection(int fd)
{
	std::cout << "Handle existing Connectionnnnnnnnnnnnnnnnnnnnnnnnnnnn" << std::endl;
	char buffer[1024];
	memset(buffer, 0, sizeof(buffer)); //-> clear the buffer
	while (true)
	{
		std::cout << "before recv this is fd: " << fd << std::endl;
		int bytes = recv(fd, buffer, sizeof(buffer), 0);
		if (bytes == -1)
		{
			std::cout << "bytes == " << bytes << std::endl;
			std::cerr << "recv() error: " << strerror(errno) << std::endl;
			// Client disconnected or error
			if (errno == EINTR)
				return;
			if (errno == EAGAIN || errno == EWOULDBLOCK)
				return;
			else
			{
				std::cout << "ERROR on recv" << std::endl;
				close(fd);
				fd = -1; // Mark for removal
				return;
				// Error
			}
		}
		if (bytes == 0)
		{
			std::cout << "Client disconnected" << std::endl;
			close(fd);
			fd = -1; // Mark for removal
			return;
			clearClients(fd);
		}
		else
		{
			std::cout << "Here we should be handling incoming data..." << std::endl;
			std::cout << "Number of received bytes: " << bytes << std::endl;
			std::cout << "Received data: " << std::string(buffer, bytes) << std::endl;

			// Handle data...
		}
	}
}

void Server::startServer()
{
	//char buffer[1024];
	std::cout << "Starting server .." << std::endl;
    while (!_signal)
    {
			int ready = pollFds();
			if (ready == -1)
			{
				 perror("poll");
				 break;
			}
			for (std::vector<struct pollfd>::iterator it = _fds.begin(); it != _fds.end();)
			{
				if (it->revents & POLLIN)
				{
				clientAccept();
				std::cout << "New Iteration of it ()" << std::endl;
				//	 bzero(buffer, sizeof(buffer));
				//	 recv(it->fd, buffer, sizeof(buffer), 0);
					if (it->fd == _socket)
					{
						 handleNewConnection();
						 break;
					}
					else
					{
						std::cout << "before Handleexistingco... it->fd: " << it->fd << std::endl;
						handleExistingConnection(it->fd);
						break;
					}
				}
				//if (it->fd == -1)
				//{
				//	it = _fds.erase(it);
				//}
				else
				{
					++it;
				}
			}
     }
		closeFds();
}

void Server::closeFds()
{
	for(size_t i = 0; i < _clients.size(); i++){ //-> close all the clients
		std::cout << "Client <" << _clients[i].getFd() << "> Disconnected" << std::endl;
		close(_clients[i].getFd());
	}
	if (_socket != -1){ //-> close the server socket
		std::cout << "Server <" << _socket << "> Disconnected" << std::endl;
		close(_socket);
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

void Server::initializeHints()
{
	struct addrinfo hints;			  // freed by itself because it is a local variable
	memset(&hints, 0, sizeof(hints)); // hints are better to be local variable since they are just used once
	hints.ai_family = AF_INET;		  // use IPv4 or IPv6, AF_INET or AF_INET6 , AF_UNSPEC is the most flexible, but might need to be changed due to allegedly not working and being unsafe
	hints.ai_socktype = SOCK_STREAM;  // use TCP, which guarantees delivery
	hints.ai_flags = AI_PASSIVE;
	std::string str = std::to_string(_port);				  // transforming int _port to const char*
	const char *cstr = str.c_str();							  // getaddrinfo resolves a hostname and service name (like a port number) into a list of address structures. These structures can then be used directly with socket functions such as socket, bind, connect, sendto, and recvfrom.
	int status = getaddrinfo(NULL, cstr, &hints, &_servInfo); // When the first argument to getaddrinfo is NULL, it indicates that you are not specifying a particular IP address to bind to. Instead, it allows the system to automatically select the appropriate IP address based on the hints you provide.
	if (status != 0)
	{
		errorPrintGetaddrinfo(1);
		exit(1);
	}
}

void Server::createSocket()
{
	_socket = socket(_servInfo->ai_family, _servInfo->ai_socktype, _servInfo->ai_protocol);
	 std::cout << "_socket: " << _socket << std::endl;
	if (_socket == -1)
    {
        errorSocketCreation(errno);
        exit(1);
    }
}
void Server::setSocketReusable()
{
    int reuse = 1;
    int result = setsockopt(_socket, SOL_SOCKET, SO_REUSEADDR, &reuse, sizeof(reuse));
    if (result == -1)
    {
       errorSetsockopt(errno);
	   close(_socket); // Close the socket on error
	   exit(1);		   // not sure if i should return 1 or exit(1)
    }
}
void Server::nonBlockingSocket()
{

   if (fcntl(_socket, F_SETFL, O_NONBLOCK) == -1)
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
 void Server::initialize_pollfd()
 {
    struct pollfd newPoll;
     newPoll.fd = _socket;   // the socket we are listening on
     newPoll.events = POLLIN; // wait for an incoming connection
     newPoll.revents = 0; // set revents to 0
     _fds.push_back(newPoll); // add the socket to the pollfd vector
 }

int Server::createAndSetSocket() // may split into smaller fumnctions
{
    createSocket();
    //std::cout  <<"After created socket"  << std::endl;
    // Set the socket to be reusable, so we can bind it again even if it is in TIME_WAIT state
    setSocketReusable();
    // Set the socket to non-blocking mode
    nonBlockingSocket();
    // Bind the socket to the port
    bindSocket();
    // Start listening on the socket
    listenSocket();
		// Start the loop waiting for connections
    initialize_pollfd();
    return (0);
}


int Server::getSocket() const
{
    return (_socket);
}
bool Server::_signal = false;
void Server::signalHandler(int signum)
{
    std::cout  << "Signal received: " << signum  << std::endl;
		_signal = true;

}

void Server::printPassword()
{
    std::cout << "Password: " << _password << std::endl;
}

void Server::printclientfds(std::vector<struct Client> clients)
{
	int i = 0;
	for(std::vector<struct Client>::iterator it = clients.begin(); it != clients.end();)
	{
		std::cout << "Client " << i << " fd: " << it->getFd() << std::endl;
		it++;
		i++;
	}
}

void Server::printfds(std::vector<struct pollfd> fds)
{
	for(std::vector<struct pollfd>::iterator it = fds.begin(); it != fds.end(); it++)
	{
		std::cout <<  " fd: " << it->fd << std::endl;
	}
}

int Server::clientAccept()
{
	struct sockaddr_in	addr;
	Client cli;
	int clientSocket = accept(_socket, (struct sockaddr *)&addr, &addrLength);
	if (clientSocket < 0) {
		std::cerr <<"Error accepting client" << std::endl;
		return (1);
	}
	if (fcntl(clientSocket, F_SETFL, O_NONBLOCK) < 0) {
		std::cerr << "Error setting client socket to non-blocking" << std::endl;
		return (1);
	}

	struct pollfd	newClientFD;
	newClientFD.fd = clientSocket;
	newClientFD.events = POLLIN;
	newClientFD.revents = 0;
	cli.setFd(clientSocket);					  //-> set the client file descriptor
	//cli.setIpAdd(inet_ntoa(clientAddr.sin_addr));
	//Client	newClient(clientSocket);

	_clients.push_back(cli);
	_fds.push_back(newClientFD);
	return (0);
}

