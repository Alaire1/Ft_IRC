#include "Server.hpp"
#include <asm-generic/socket.h>
#include <netdb.h>
#include <string>
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


 void Server::pollFds()
 {
     int timeout = 1000; // 1 second timeout // random value
     int ready = poll(&_fds[0], _fds.size(), timeout);
     if (ready == -1)
     {
         errorPoll(errno);
         exit(1);
     }
 }

 void Server::handleNewConnection() {
	 std::cout << "inside new Connection" << std::endl;
     Client cli;
     struct sockaddr_in clientAddr;
     socklen_t addrLen = sizeof(clientAddr);
     int newFd = accept(_socket, (struct sockaddr*)&clientAddr, &addrLen);

		 if (newFd == -1) 
		 {
			 //errorAccept(errno);
			 std::cout << "error accept"  << std::endl;
			 return;
		 }
		 else
			 std::cout << "Accept success!!" << std::endl;
     addFd(newFd, POLLIN);
     cli.setFd(newFd);//-> set the client file descriptor
 		 cli.setIpAdd(inet_ntoa(clientAddr.sin_addr)); //-> convert the ip address to string and set it
 		 _clients.push_back(cli); //-> add the client to the vector of clients
 }

 void Server::addFd(int newFd, short events)
 {
			struct pollfd newPoll;
			newPoll.fd = newFd; //-> set the client file descriptor
			newPoll.events = events; //
			newPoll.revents = 0;  //-> add the client to the vector of clients
			_fds.push_back(newPoll); //-> add the client socket to the pollfd
 }

void Server::handleExistingConnection(int fd) {
    std::cout << "inside existing Connection" << std::endl;
    char buffer[1024];
    memset(buffer, 0, sizeof(buffer)); // Clear the buffer

    while (true) {
        int bytes = recv(fd, buffer, sizeof(buffer), 0);

        if (bytes == -1) {
            std::cout << "Bytes is " << bytes << std::endl;
            std::cerr << "recv() error: " << strerror(errno) << std::endl;

            if (errno == EINTR) {
                // Interrupted by a signal, try again
                continue;
            } else if (errno == EAGAIN || errno == EWOULDBLOCK) {
                // No data available for non-blocking socket
                break;
            } else {
                // Other errors
                std::cout << "Error on recv and negative bytes" << std::endl;
                close(fd);
                fd = -1; // Mark for removal
                break;
            }
        } else if (bytes == 0) {
            // Client disconnected
            std::cout << "Client disconnected" << std::endl;
            close(fd);
            fd = -1; // Mark for removal
            break;
        } else {
            // Handle the received data
            std::cout << "Here we should be handling incoming data..." << std::endl;
            std::cout << "Received data: " << std::string(buffer, bytes) << std::endl;
        }
    }
}
//void Server::handleExistingConnection(int fd) {
//	 std::cout << "inside existing Connection" << std::endl;
//		char buffer[1024];
//		memset(buffer, 0, sizeof(buffer)); //-> clear the buffer
//		int bytes = recv(fd, buffer, sizeof(buffer), 0);
//		if (bytes == -1)
//			std::cout << "Bytes is -1" << std::endl;
//		while (1)
//		{
//			if (bytes <= 0) 
//			{
//				// Client disconnected or error
//				if (bytes == 0) 
//				{
//					//clearClients(fd);
//					continue;
//				} 
//				else 
//				{
//					std::cout << "Error on recv and negative bytes" << std::endl;
//					break;
//					// Error
//				}
//				close(fd);
//				fd = -1; // Mark for removal
//			} 
//			else 
//			{
//				std::cout << "Here we should be handling incoming data..." << std::endl;
//				// Handle data...
//			}
//		}
////		if (bytes <= 0) {
////			// Client disconnected or error
////			if (bytes == 0) {
////				clearClients(fd);
////			} else {
////				std::cout << "Error on recv and negative bytes" << std::endl;
////				// Error
////			}
////			close(fd);
////			fd = -1; // Mark for removal
////		} else {
////				std::cout << "Here we should be handling incoming data..." << std::endl;
////			// Handle data...
////		}
//}
 void Server::startServer()
 {
     //_signal = true;
	 std::cout << "Starting server .." << std::endl;
     while (_signal == false)// maybe while(_signal == false)
     {
			 pollFds();
			 for (std::vector<struct pollfd>::iterator it = _fds.begin(); it != _fds.end(); ) {
				 if (it->revents & POLLIN) 
				 {
					 if (it->fd == _socket) 
					 {
						 handleNewConnection();
					 } 
					 else 
					 {
						 handleExistingConnection(it->fd);
					 }
				 }
				 if (it->fd == -1) {
					 it = _fds.erase(it);
				 } 
				 else 
				 {
					 //std::cout << "it revents: " << it->revents << std::endl;
					 std::cout << "it : " << it->fd << std::endl;
					 //std::cout << "it events: " << it->events << std::endl;
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
			break;
		}
		else
			it++;
	}
	for (std::vector<struct Client>::iterator it = _clients.begin(); it != _clients.end();)
	{
		if (it->getFd() == fd)
		{
			_clients.erase(it);
			break;
		}
		else
			it++;
	}
}

void Server::initializeHints()
{
    struct addrinfo hints;  // freed by itself because it is a local variable
    memset(&hints, 0, sizeof(hints)); // hints are better to be local variable since they are just used once 
    hints.ai_family = AF_INET; // use IPv4 or IPv6, AF_INET or AF_INET6 , AF_UNSPEC is the most flexible, but might need to be changed due to allegedly not working and being unsafe
    hints.ai_socktype = SOCK_STREAM; // use TCP, which guarantees delivery
		hints.ai_flags = AI_PASSIVE;
		std::string str = std::to_string(_port); //transforming int _port to const char*
		const char* cstr = str.c_str(); // getaddrinfo resolves a hostname and service name (like a port number) into a list of address structures. These structures can then be used directly with socket functions such as socket, bind, connect, sendto, and recvfrom.
		int status = getaddrinfo(NULL, cstr, &hints, &_servInfo);//When the first argument to getaddrinfo is NULL, it indicates that you are not specifying a particular IP address to bind to. Instead, it allows the system to automatically select the appropriate IP address based on the hints you provide. 
    if (status != 0) 
		{
        errorPrintGetaddrinfo(1);
        exit(1);
    }
}

void Server::createSocket()
{
	//std::cout << "in createSocket" << std::endl;
//	std::cout << _servInfo->ai_family << std::endl;
//	std::cout << _servInfo->ai_socktype << std::endl;
//	std::cout << _servInfo->ai_protocol << std::endl;
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
        exit (1); // not sure if i should return 1 or exit(1) 
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
        //close(_socket); // Close the socket on error
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
bool Server::_signal = false; //-> initialize the static boolean
void Server::signalHandler(int signum)
{
    std::cout  << "Signal received: " << signum  << std::endl;
		Server::_signal = true; //-> set the static boolean to true to stop the server
}
 
void Server::printPassword()
{
    std::cout << "Password: " << _password << std::endl;
}
