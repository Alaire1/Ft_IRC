#include "Server.hpp"
#include <cstdlib>


Server::Server() : _socket(-1),  _port(6667), _password("1234"), _servInfo(NULL) {}

Server::Server(int port, std::string password) :  _socket(-1), _port(port) ,_password(password) {}

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
void Server::handle_sigstop(int sig)
{
    if (sig == SIGTSTP)
    {
        std::cout << "SIGTSTP received. Stopping server..." << std::endl;
        _signal = true;
    }
}
void Server::handleSignals()
{
    // signal(SIGINT, signalHandler);
	signal(SIGTSTP, handle_sigstop);
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
		std::cout << "error accept " << strerror(errno) << std::endl;
		return;
	}
	if (fcntl(clientSocket, F_SETFL, O_NONBLOCK) < 0)
	{
		std::cerr << "Error setting client socket to non-blocking"  << std::endl;
		close(clientSocket);
		return;
	}
	//std::cout << "Accept success!!" << std::endl;
	addFd(clientSocket, POLLIN);
	cli.setFd(clientSocket);					  //-> set the client file descriptor
	_clients.push_back(cli);					  //-> add the client to the vector of clients
	std::cout << "client socket added to _clients!!" << std::endl;
	//printclientfds(_clients);
	//std::cout << _clients.size() << std::endl;
 }

 void Server::addFd(int newFd, short events)
 {
	std::cout << "in addFD Client socket: " << newFd << std::endl;
	struct pollfd newPoll;
	newPoll.fd = newFd;		 //-> set the client file descriptor
	newPoll.events = events; //
	newPoll.revents = 0;	 //-> add the client to the vector of clients
	_fds.push_back(newPoll); //-> add the client socket to the pollfd
	std::cout << "Client socket [" << newFd << "] added to _fds!"<< std::endl;
	//printfds(_fds);
 }

void Server::handleExistingConnection(int fd)
{
	std::cout << "Handle existing Connectionnnnnnnnnnnnnnnnnnnnnnnnnnnn" << std::endl;

	char buffer[1024];
	memset(buffer, 0, sizeof(buffer));
	std::cout << "before recv this is fd: " << fd << std::endl;
	int bytes = recv(fd, buffer, sizeof(buffer), 0);
	if (bytes < 0)
	{
		std::cerr << "bytes == " << bytes << std::endl;
		std::cerr << "recv() error: " << strerror(errno) << std::endl;
		close(fd);
		fd = -1;
	}
	else if (bytes == 0)
	{
		std::cerr << "Client disconnected" << std::endl;
		clearClients(fd);
		close(fd);
		fd = -1;
	}
	else
	{
		buffer[bytes] = '\0';
		std::cout << "Here we should be handling incoming data..." << std::endl;
		std::cout << "[Client " << fd << " ]";
		std::cout << "Received data: " << std::string(buffer, bytes) << std::endl;

	}
	closeFds();
}

void Server::startServer()
{
	std::cout << "Server < " << _socket << " > waiting for connection... " << std::endl;


	while (!Server::_signal)
	{
		if ((poll(&_fds[0], _fds.size(), -1) == -1) && !_signal)
		{
			std::cerr << "Error polling" << std::endl;
			break;
		}
		for (size_t i = 0; i < _fds.size(); i++)
		{
			//std::cout << "Checking revents for fd: " << _fds[i].fd << std::endl;
			if (_fds[i].revents & POLLIN)
			{
				std::cout << "POLLIN event detected on fd: " << _fds[i].fd << std::endl;
				if(_fds[i].fd == _socket)
				{
					std::cout << "Client accepted" << std::endl;
					handleExistingConnection(_fds[i].fd);
					break;
				}
				else
				{
					std::cout << "Handling existing connection" << std::endl;
					handleExistingConnection(_fds[i].fd); // reading data from client
					break;
				}
			}
		}
	}
	closeFds();//close file descriptors and clients vector
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
		exit(EXIT_FAILURE);
	}
}

//void Server::createSocket()
//{
//	//_socket = socket(_servInfo->ai_family, _servInfo->ai_socktype, _servInfo->ai_protocol);
//	_socket = socket(AF_INET, SOCK_STREAM, 0);
//	//std::cout << "_socket: " << _socket << std::endl;
//	if (_socket == -1)
//	{
//		errorSocketCreation(errno);
//		exit(1);
//	}
//}
//
//void Server::setSocketReusable()
//{
//	int reuse = 1;
//	int result = setsockopt(_socket, SOL_SOCKET, SO_REUSEADDR, &reuse, sizeof(reuse));
//	if (result == -1)
//	{
//		errorSetsockopt(errno);
//		close(_socket); // Close the socket on error
//		exit(1);		   // not sure if i should return 1 or exit(1)
//	}
//}
//
//void Server::nonBlockingSocket()
//{
//
//	if (fcntl(_socket, F_SETFL, O_NONBLOCK) == -1)
//	{
//		errorFcntl(errno);
//		close(_socket); // Close the socket on error
//		exit(1);
//	}
//
//}
//
//void Server::bindSocket()
//{
//	struct sockaddr_in add;
//	add.sin_family = AF_INET; //-> set the address family to ipv4
//	add.sin_addr.s_addr = INADDR_ANY; //-> set the address to any local machine address
//	add.sin_port = htons(this->_port); //-> convert the port to network byte order (big endian)
//	if (bind(_socket, (struct sockaddr *)&add, sizeof(add)) == -1)
//	{
//		errorSocketBinding(errno);
//		close(_socket); // Close the socket on error
//		exit(1);
//	}
//}
//
//void Server::listenSocket()
//{
//	// BACKLOG is the number of connections that can be waiting while the process is handling a particular connection
//	if (listen(_socket, SOMAXCONN) == -1) {
//		errorListen(errno);
//		close(_socket); // Close the socket on error
//		exit(1);
//	}
//}
//
//void Server::initialize_pollfd()
//{
//	struct pollfd newPoll;
//	newPoll.fd = _socket;   // the socket we are listening on
//	newPoll.events = POLLIN; // wait for an incoming connection
//	newPoll.revents = 0; // set revents to 0
//	_fds.push_back(newPoll); // add the socket to the pollfd vector
//}

void Server::createAndSetSocket() // may split into smaller fumnctions
{
	_socket = socket(AF_INET, SOCK_STREAM, 0);
	if (_socket == -1)
	{
		errorSocketCreation(errno);
		exit(EXIT_FAILURE);
	}
    //createSocket();

	int reuse = 1;
	if (setsockopt(_socket, SOL_SOCKET, SO_REUSEADDR, &reuse, sizeof(reuse)) < 0)
	{
		errorSetsockopt(errno);
		close(_socket); // Close the socket on error
		exit(EXIT_FAILURE);		   // not sure if i should return 1 or exit(1)
	}
    //setSocketReusable();

	if (fcntl(_socket, F_SETFL, O_NONBLOCK) == -1)
	{
		errorFcntl(errno);
		close(_socket); // Close the socket on error
		exit(1);
	}
    //nonBlockingSocket();

	struct sockaddr_in add;
	add.sin_family = AF_INET; //-> set the address family to ipv4
	add.sin_addr.s_addr = INADDR_ANY; //-> set the address to any local machine address
	add.sin_port = htons(_port); //-> convert the port to network byte order (big endian)
	if (bind(_socket, (struct sockaddr *)&add, sizeof(add)) == -1)
	{
		errorSocketBinding(errno);
		close(_socket); // Close the socket on error
		exit(EXIT_FAILURE);
	}
    //bindSocket();

	if (listen(_socket, SOMAXCONN) == -1) {
		errorListen(errno);
		close(_socket); // Close the socket on error
		exit(EXIT_FAILURE);
	}
    //listenSocket();

	struct pollfd newPoll;
	newPoll.fd = _socket;   // the socket we are listening on
	newPoll.events = POLLIN; // wait for an incoming connection
	newPoll.revents = 0; // set revents to 0
	_fds.push_back(newPoll); // add the socket to the pollfd vector
    //initialize_pollfd();
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
	std::cout << "Print clients" << std::endl;
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
	std::cout << "Print fds" << std::endl;
	for(std::vector<struct pollfd>::iterator it = fds.begin(); it != fds.end(); it++)
	{
		std::cout <<  " fd: " << it->fd << std::endl;
		//std::cout << " revents: " << it->revents << std::endl;
	}
}

//void Server::clientAccept()
//{
//	Client cli;
//	struct pollfd	newClientFD;
//	struct sockaddr_in	addr;
//
//	socklen_t			addrLength = sizeof(addr);
//	int clientSocket = accept(_socket, (struct sockaddr *)&(addr), &addrLength);
//	if (clientSocket < 0) 
//	{
//		std::cerr <<"Error accepting client" << std::endl;
//		return;
//	}
//	if (fcntl(clientSocket, F_SETFL, O_NONBLOCK) < 0) 
//	{
//		std::cerr << "Error setting client socket to non-blocking" << std::endl;
//		return;
//	}
//	newClientFD.fd = clientSocket;
//	newClientFD.events = POLLIN;
//	newClientFD.revents = 0;
//	_fds.push_back(newClientFD);
//	//Add the newClients FD to the clients vector
//	cli.setFd(clientSocket);  //-> set the client file descriptor
//	cli.setIpAdd(inet_ntoa(addr.sin_addr));
//	_clients.push_back(cli);
//	//Printintg clients and fds
//	std::cout << "Client socket: " << clientSocket << " added to the list"<<  std::endl;
//	printclientfds(_clients);
//	printfds(_fds);
//}


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

//ctrl v + m  ctrl v + j
