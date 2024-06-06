#include "Server.hpp"
#include <cstddef>

Server::Server(){}

Server::Server(int port, std::string password) : _serverFd(-1), _port(port), _pwd(password){}

Server::~Server(){}

void Server::signalHandler(int signum)
{
	(void)signum;
	std::cout << "SIGTSTP received. Stopping server..." << std::endl;
	//_signal = true;

}

int Server::getSocket() const
{
    return (_serverFd);
}

void Server::initServer()
{
	createSocket();
	runServer();
}

void Server::setNonBlocking(int socket) {
    int flags = fcntl(socket, F_GETFL, 0);
    fcntl(socket, F_SETFL, flags | O_NONBLOCK);
}

void Server::createSocket() 
{
    struct sockaddr_in serverAddr;

    // Create server socket
    _serverFd = socket(AF_INET, SOCK_STREAM, 0);
    if (_serverFd < 0) 
			throw(std::runtime_error("failed to create socket"));

    // Set socket options
    int opt = 1;
    if (setsockopt(_serverFd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0) 
			throw(std::runtime_error("failed to set SO_REUSEADDR on socket"));

    // Set server address
    memset(&serverAddr, 0, sizeof(serverAddr));
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_addr.s_addr = INADDR_ANY;
    serverAddr.sin_port = htons(_port);

    // Bind socket
    if (bind(_serverFd, (struct sockaddr*)&serverAddr, sizeof(serverAddr)) < 0) 
			throw(std::runtime_error("failed to bind socket"));

    // Listen for connections
    if (listen(_serverFd, MAX_CLIENTS) < 0) 
			throw(std::runtime_error("listen() failed"));

    // Set server socket to non-blocking
    setNonBlocking(_serverFd);

    // Add server socket to poll list
    struct pollfd serverPollFd;
    serverPollFd.fd = _serverFd;
    serverPollFd.events = POLLIN;
    _fds.push_back(serverPollFd);
}

void Server::runServer()
{
	std::cout << "Server listening on port " << _port << std::endl;

	while (true) {
		// Poll sockets
		int pollCount = poll(_fds.data(), _fds.size(), -1);
		if (pollCount < 0) 
		{
			throw(std::runtime_error("poll() failed"));
			break;
		}

		for (size_t i = 0; i < _fds.size(); ++i) 
		{
			if (_fds[i].revents & POLLIN) 
			{
				if (_fds[i].fd == _serverFd)
					acceptClient();
				else 
					handleData(_fds[i].fd, i);
			}
		}
	}
	// Close server socket
	close(_serverFd);
}

void Server::handleData(int fd, size_t idx)
{
	char buffer[BUFFER_SIZE];
	memset(buffer, 0, BUFFER_SIZE);
	int bytesRead = recv(fd, buffer, BUFFER_SIZE, 0);
	if (bytesRead <= 0) 
	{
		if (bytesRead == 0) 
			std::cout << "Client disconnected (fd: " << fd << ")" << std::endl;
		else 
			std::cerr << "ERROR reading from socket (fd: " << fd << ")" << std::endl;

		close(fd);
		_fds.erase(_fds.begin() + idx);
		--idx; // Adjust index after erasing
	} 
	else 
	{
		std::cout << "Received message: " << buffer << " from fd: " << fd << std::endl;
		// Echo message back to client
		send(fd, buffer, bytesRead, 0);
	}

}

void Server::acceptClient()
{
	int newSocket;
	struct sockaddr_in clientAddr;
	socklen_t addrLen = sizeof(clientAddr);

	newSocket = accept(_serverFd, (struct sockaddr*)&clientAddr, &addrLen);

	if (newSocket < 0) 
		std::cerr << "ERROR on accept" << std::endl;
	else 
	{
		std::cout << "New connection accepted (fd: " << newSocket << ")" << std::endl;
		setNonBlocking(newSocket);
		struct pollfd clientPollFd;
		clientPollFd.fd = newSocket;
		clientPollFd.events = POLLIN;
		_fds.push_back(clientPollFd);
		Client cli;
		cli.setFd(newSocket);
		_clients.push_back(cli);
	}
}


void Server::closeFds()
{
	for(size_t i = 0; i < _fds.size(); i++)
	{ //-> close all the clients
		if (_fds[i].fd  == -1)
		{
			std::cout << "Close fd <" << _fds[i].fd  << std::endl;
			close(_fds[i].fd);
			_fds[i].fd = -1;
		}
	}
	_fds.clear();
	_clients.clear();
}


void Server::printPassword()
{
    std::cout << "Password: " << _pwd << std::endl;
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

bool Server::isSocketClosed(int sockfd) 
{
    int error = 0;
    socklen_t len = sizeof(error);
    int retval = getsockopt(sockfd, SOL_SOCKET, SO_ERROR, &error, &len);
    
    if (retval != 0) 
		{
        // There was a problem getting the error code
        std::cerr << "getsockopt() failed: " << strerror(errno) << std::endl;
        return true;
    }

    if (error != 0) 
		{
        // An error has been detected
        std::cerr << "socket error: " << strerror(error) << std::endl;
        return true;
    }
    return false;
}


//int	Server::clearEmptyChannels()
//{
//	for (channelIt it = _channels.begin(); it != _channels.end(); ++it) {
//		if (it->getUserCount() == 0) {
//			_channels.erase(it--);
//		}
//	}
//	return (1);
//}


