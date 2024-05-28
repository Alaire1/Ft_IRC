#include "Server.hpp"
#include <netinet/in.h>
#include <sys/types.h>

bool Server::_signal = false;

Server::Server() : _servSocket(-1),  _port(6667), _password("1234"){}

Server::Server(int port, std::string password) :  _servSocket(-1), _port(port) ,_password(password) {}

Server::~Server() 
{
	freeaddrinfo(NULL);
}

void Server::signalHandler(int signum)
{
	(void)signum;
	std::cout << "SIGTSTP received. Stopping server..." << std::endl;
	_signal = true;

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

void Server::createServer()
{
	createAndSetSocket();
	startServer();
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
	struct sockaddr_in newcli;
	socklen_t addrLen = sizeof(newcli);

	std::cout << "server socket fd: " << _servSocket << std::endl;//testing
	int clientSocket = accept(_servSocket, (sockaddr*)&newcli, &addrLen);
	std::cout << "New fd: " << clientSocket << std::endl;//testing
	if (clientSocket == -1)
	{
		std::cerr << "error accept " << strerror(errno) << std::endl;
		return;
	}
	if (fcntl(clientSocket, F_SETFL, O_NONBLOCK) == -1)
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
	struct sockaddr_in addr;
	socklen_t addr_len = sizeof(addr);
	if (getpeername(fd, (struct sockaddr*)&addr, &addr_len) == -1) {
		std::cerr << "Socket " << fd << " is not connected. Error: " << strerror(errno) << std::endl;
		clearClients(fd);
		close(fd);
		return;
	}
	std::cout << "before recv this is fd: " << fd << std::endl;
	ssize_t bytes = recv(fd, buffer, sizeof(buffer) - 1, 0);
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
		close(fd);
		fd = -1;
		clearClients(fd);
	}
	else
	{
		buffer[bytes] = '\0';
		std::cout << "Here we should be handling incoming data..." << std::endl;
		std::cout << "[Client " << fd << " ] ";
		std::cout << "Received data: " << std::string(buffer, bytes) << std::endl;
	}
}

void Server::startServer()
{
	std::cout << "Server < " << _servSocket << " > waiting for connection... " << std::endl;

	if(isSocketClosed(_servSocket))
		std::cerr << "Socket closed" << std::endl;
	while (!Server::_signal)
	{
		if ((poll(&_fds[0], _fds.size(), -1) == -1) && !Server::_signal)
		{
			//probably a good idea to use throw();
			std::cerr << "Error polling" << std::endl;
			break;
		}
		for (size_t i = 0; i < _fds.size(); i++)
		{
			//std::cout << "Checking revents for fd: " << _fds[i].fd << std::endl;
			if (_fds[i].revents & POLLIN)
			{
				std::cout << "POLLIN event detected on fd: " << _fds[i].fd << std::endl;
				if(_fds[i].fd == _servSocket)
				{
					handleNewConnection();
					//break;
				}
				else
				{
					handleExistingConnection(_fds[i].fd); // reading data from client
					//break;
				}
			}
		}
	}
	std::cout << "Loop broken " << std::endl;
	std::cout << "Server fd: " << _servSocket << std::endl;
	closeFds();//close file descriptors and clients vector
}


//void Server::createSocket()
//{
//	//_servSocket = socket(_servInfo->ai_family, _servInfo->ai_socktype, _servInfo->ai_protocol);
//	_servSocket = socket(AF_INET, SOCK_STREAM, 0);
//	//std::cout << "_servSocket: " << _servSocket << std::endl;
//	if (_servSocket == -1)
//	{
//		errorSocketCreation(errno);
//		exit(1);
//	}
//}
//
//void Server::setSocketReusable()
//{
//	int reuse = 1;
//	int result = setsockopt(_servSocket, SOL_SOCKET, SO_REUSEADDR, &reuse, sizeof(reuse));
//	if (result == -1)
//	{
//		errorSetsockopt(errno);
//		close(_servSocket); // Close the socket on error
//		exit(1);		   // not sure if i should return 1 or exit(1)
//	}
//}
//
//void Server::nonBlockingSocket()
//{
//
//	if (fcntl(_servSocket, F_SETFL, O_NONBLOCK) == -1)
//	{
//		errorFcntl(errno);
//		close(_servSocket); // Close the socket on error
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
//	if (bind(_servSocket, (struct sockaddr *)&add, sizeof(add)) == -1)
//	{
//		errorSocketBinding(errno);
//		close(_servSocket); // Close the socket on error
//		exit(1);
//	}
//}
//
//void Server::listenSocket()
//{
//	// BACKLOG is the number of connections that can be waiting while the process is handling a particular connection
//	if (listen(_servSocket, SOMAXCONN) == -1) {
//		errorListen(errno);
//		close(_servSocket); // Close the socket on error
//		exit(1);
//	}
//}
//
//void Server::initialize_pollfd()
//{
//	struct pollfd newPoll;
//	newPoll.fd = _servSocket;   // the socket we are listening on
//	newPoll.events = POLLIN; // wait for an incoming connection
//	newPoll.revents = 0; // set revents to 0
//	_fds.push_back(newPoll); // add the socket to the pollfd vector
//}

void Server::createAndSetSocket() // may split into smaller fumnctions
{
	struct pollfd newPoll;
	struct sockaddr_in hints;			  // freed by itself because it is a local variable
	memset(&hints, 0, sizeof(hints)); // hints are better to be local variable since they are just used once
	hints.sin_family = AF_INET;		  // use IPv4 or IPv6, AF_INET or AF_INET6 , AF_UNSPEC is the most flexible, but might need to be changed due to allegedly not working and being unsafe
	hints.sin_port = htons(this->_port);  // use TCP, which guarantees delivery
	hints.sin_addr.s_addr = INADDR_ANY;

	_servSocket = socket(AF_INET, SOCK_STREAM, 0);
	if (_servSocket == -1)
		errorSocketCreation(errno);

	int reuse = 1;
	if (setsockopt(_servSocket, SOL_SOCKET, SO_REUSEADDR, &reuse, sizeof(reuse)) < 0)
	{
		errorSetsockopt(errno);
		//close(_servSocket); // Close the socket on error
		//exit(EXIT_FAILURE);		   // not sure if i should return 1 or exit(1)
	}
	if (fcntl(_servSocket, F_SETFL, O_NONBLOCK) == -1)
	{
		errorFcntl(errno);
		//close(_servSocket); // Close the socket on error
		//exit(1);
	}
	if (bind(_servSocket, (struct sockaddr *)&hints, sizeof(hints)) == -1)
	{
		errorSocketBinding(errno);
		//close(_servSocket); // Close the socket on error
		//exit(EXIT_FAILURE);
	}
	if (listen(_servSocket, SOMAXCONN) == -1) {
		errorListen(errno);
		close(_servSocket); // Close the socket on error
		exit(EXIT_FAILURE);
	}
    //listenSocket();

	newPoll.fd = _servSocket;   // the socket we are listening on
	newPoll.events = POLLIN; // wait for an incoming connection
	newPoll.revents = 0; // set revents to 0
	_fds.push_back(newPoll); // add the socket to the pollfd vector
}



int Server::getSocket() const
{
    return (_servSocket);
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
//	int clientSocket = accept(_servSocket, (struct sockaddr *)&(addr), &addrLength);
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
	_fds.clear();

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
	_clients.clear();
}

bool Server::isSocketClosed(int sockfd) 
{
    int error = 0;
    socklen_t len = sizeof(error);
    int retval = getsockopt(sockfd, SOL_SOCKET, SO_ERROR, &error, &len);
    
    if (retval != 0) {
        // There was a problem getting the error code
        std::cerr << "getsockopt() failed: " << strerror(errno) << std::endl;
        return true;
    }

    if (error != 0) {
        // An error has been detected
        std::cerr << "socket error: " << strerror(error) << std::endl;
        return true;
    }

    return false;
}

//ctrl v + m  ctrl v + j
