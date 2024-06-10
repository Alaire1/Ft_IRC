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
					handleData(_fds[i].fd, _clients[i - 1], i);
			}
		}
	}
	// Close server socket
	close(_serverFd);
}

void Server::handleData(int fd, Client &sender, size_t idx)
{
	char buffer[BUFFER_SIZE];
	memset(buffer, 0, BUFFER_SIZE);
	int bytesRead = recv(fd, buffer, BUFFER_SIZE, 0);
	if (bytesRead <= 0) 
	{
		if (bytesRead == 0) 
		{
			std::cout << "Client disconnected (fd: " << fd << ")" << std::endl;
			clearChannelsNoUsers();
		}
		else 
			std::cerr << "ERROR reading from socket (fd: " << fd << ")" << std::endl;

		close(fd);
		_fds.erase(_fds.begin() + idx);
		--idx; // Adjust index after erasing
	} 
	else 
	{
		//std::cout << "Received message: " << buffer;// << " from fd: " << fd << std::endl;
		
		parseCommand(buffer, sender);
		// Echo message back to client
		//send(fd, buffer, BUFFER_SIZE, 0);
		//std::cout << "client size message: " << _clients.size() << std::endl;
	//	if(_clients.size() == 2)
	//	{
	//		const char message[BUFFER_SIZE] = ":ft_irc 001 TheOne :Welcome to ft_irc server!";

	//		size_t value = send(fd, message, BUFFER_SIZE, 0);
	//		std::cout << fd <<" Sent "<< value << " : " << message << std::endl;
	//	}
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

void Server::printclientfds(std::vector<Client> clients)
{
	std::cout << "Print clients" << std::endl;
	int i = 0;
	for(std::vector<Client>::iterator it = clients.begin(); it != clients.end();)
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


int	Server::clearChannelsNoUsers()
{
	for (std::vector<Channel>::iterator it = _channels.begin(); it != _channels.end();) 
	{
		if (it->getUsernum() == 0) 
			_channels.erase(it);
		else
			it++;
	}
	return (1);
}
//PARSING FUNCTIONS


// Client Server::findClientByFd(int fd){
// 	for (size_t i = 0; i < _clients.size(); i++) {
// 		if (_clients[i].getFd() == fd) {
// 			return &_clients[i];
// 		}
// 	}
// 	return nullptr;
// }

// void printCommands(std::vector<std::string> commands){
// 	for (const std::string& command : commands) {
// 		std::cout << command << '\n';
// 	}
// }
int Server::checkNick(std::string nick){
	  for (std::vector<Client>::iterator it = _clients.begin(); it != _clients.end(); ++it) {
            if (it->getNick() == nick) {
                std::cout << "Nick is already in use" << std::endl;
                return 0;
            }
        }
		return (1);

}
void Server::commandsRegister(Client sender, std::string command, std::string param1){
	std::cout << "Commands register " <<  std::endl;
	std::cout << "Command: " << command <<  std::endl;
	if (command == "NICK")
	{
		//std::cout << "NICK" << std::endl;
		if (checkNick(param1) == false)
			return;
		sender.setNickname(param1);
		//std::cout << "Nick: " << sender.getNick() << std::endl;
	}
	else if (command == "USER")
	{
		//std::cout << "USER" << std::endl;
		sender.setUsername(param1);
		//std::cout << "User: " << sender.getUser() << std::endl;
	}
	else if (command == "PASS")
	{
		//std::cout << "PASS" << std::endl;
		sender.hasPassword = true;
	}
	
}
void Server::commandsAll(Client sender, std::string command, std::string parameter1, std::string parameter2, std::string parameter3)
{
	(void)parameter2;
	(void)parameter3;
	(void)parameter1;
	std::cout << "Command: " << command << std::endl;
	if (command == "JOIN")
	{
		//joinChannel(parameter, message, client);
		std::cout << "JOIN" << std::endl;
	}
	// else if (command == "PART")
	// 	//part();
	// else if (command == "TOPIC")
	// 	//channelTopic(parameter, message, client);
	// else if (comamnd == "PRIVMSG")
	// 	//sendMessage(parameter, message, client);
	// else if (command == "WHO")
	// 	//names(client, parameter);
	// else if (command == "KICK")
	// 	//kickClient(parameter, parameter2, message, client);
	// else if (command == "INVITE")
	// 	//inviteChannel(parameter, parameter2, client);
	// else if (command == "MODE") 
	// 	//mode(parameter, parameter2, parameter3, client);
	else
	{
		std::string errorMessage = "421 " + sender.getNick() + " " + command + " :Unknown command\r\n";
		std::cout << errorMessage;
		// sendToClient(sender.getFd(), errorMessage);
	}
}

void Server::parseCommand(std::string clientData, Client &sender){
    std::cout << "Current client : " << sender.getFd() << std::endl;
    std::vector<std::string> commands = splitString(clientData, "\r\n");
    std::vector<std::string>::const_iterator it;
    for (it = commands.begin(); it != commands.end(); ++it) {
        std::istringstream iss(removeNonPrintable(*it));
    	std::string command, param1, param2, param3;
    	iss >> command >> param1 >> param2 >> param3;
    	//std::cout << "Command: " << command << " Param1: " << param1 << " Param2: " << param2 << " Param3: " << param3 << std::endl;
		if (sender.isRegistered == false)
			{
				commandsRegister(sender, command, param1);
				if (sender.hasPassword == true && sender.getNick() != "" && sender.getUser() != "")
				{
					sender.isRegistered = true;
					std::string welcomeMessage = ":ft_irc 001 " + sender.getNick() + " :Welcome to ft_irc server!\r\n";
					std::cout << welcomeMessage;
					// sendToClient(sender.getFd(), welcomeMessage);
				}
			}
		else
			{std::cout << "Nick: " << sender.getNick()  << std::endl;
			std::cout << "User: " << sender.getUser() << std::endl;}
		// else if (command == "QUIT")
		// {
		// 	close(sender.getFd());
		// 	//other functions for clearing all the data
		// }
		// else
		// {
		// 	//commandsAll(sender, command, param1, param2, param3);
		// 	std::cout << "Nick: " << sender.getNick()  << std::endl;
		// 	std::cout << "User: " << sender.getUser() << std::endl;
		// }
	}
}


//helper functions
std::string removeNonPrintable(const std::string& input) {
    std::string result;
    for (char c : input) {
        if (std::isprint(static_cast<unsigned char>(c))) {
            result += c;
        }
    }
    return result;
}

std::vector<std::string> splitString(std::string str, std::string delimiter) {
    std::vector<std::string> result;
    size_t pos = 0;
    std::string token;

    while ((pos = str.find(delimiter)) != std::string::npos) {
        token = str.substr(0, pos);
        result.push_back(token);
        str.erase(0, pos + delimiter.length());
    }
    result.push_back(str);
    return result;
}


