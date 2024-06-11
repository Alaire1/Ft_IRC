#include "Server.hpp"
#include "parsing_plan.cpp"
#include <cstddef>
#include <sstream>

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
		//buffer[bytesRead] = '\r';
		//std::cout << "Received message: " << buffer;// << " from fd: " << fd << std::endl;
	//	ircMessageParser(buffer, *this);
		parseCommand(buffer, sender);


		// Echo message back to client
		//if(!strncmp(buffer, "test", 4))
		//{
		//	std::string str = serverReply(SERVER, "001", {"exampleNick"}, "Welcome to ft_irc serverrrr!");

		//	//const char buf[48] = ":ft_irc 001 :Welcome to ft_irc server!\n";
		//	//send(fd, buf, 48, 0);
		//	int value = send(fd, str.c_str(), str.length(), 0);
		//	std::cout << fd <<"[fd] sent "<< value << " -> " << str << std::endl;
		//}
	}

}





bool Server::channelExists(std::string channelName)
{
	for (std::vector<Channel>::iterator it = _channels.begin(); it != _channels.end(); it++)
	{
		if (it->getChannelName() == channelName)
			return true;
	}
	return false;
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


std::string Server::serverReply(const std::string& prefix, const std::string& cmd, const std::vector<std::string>& params, const std::string& trailingParam)
{
	std::stringstream message;
	// Build the message components
		message << ":" << prefix << " " << cmd;
	//message << Channel/Client;
	// Add parameters if any
	if (!params.empty()) 
	{
		for(size_t i = 0; i < params.size(); i++)
			message << " " << params[i];
	}

	// Add trailing parameter if provided
	if (!trailingParam.empty()) {
		message << " :" << trailingParam;
	}
	message << "\r\n";
	// Return the complete server reply string
	return message.str();
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


int	Server::sendToClient(const std::string& message, const Client& client) const
{
	if (send(client.getFd(), message.c_str(), message.length(), 0) < 0)
	{
		std::cout << "Error sending message"  << std::endl;
		return (-1);
	}
	std::cout << "Sent: " << message << " to " << client.getNick() << std::endl;
	return (0);
}
void Server::commandsRegister(Client& sender, std::string command, std::string param1){
	if (command == "NICK")
	{
		if (checkNick(param1) == false)
			return;
		sender.setNickName(param1);
	}
	else if (command == "USER")
	{
		sender.setUserName(param1);
	}
	else if (command == "PASS")
	{
		if (param1 == _pwd)
			sender.setHasPassword(true);
		else
		{
			std::string errorMessage = serverReply(SERVER, "464", {sender.getNick()}, "Password wjdhwdw dwdrjbwgyufw fnwe4vgfyfr");
			sendToClient(errorMessage, sender); // we have to handle errors while sending
		}
	}
	else if (command == "QUIT")
		{
			close(sender.getFd());

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
		//sendToClient(sender.getFd(), errorMessage);
	}
}

void Server::parseCommand(std::string clientData, Client& sender){
	std::cout << "Current client : " << sender.getFd() << std::endl;
	std::vector<std::string> commands = splitString(clientData, "\r\n");
	std::vector<std::string>::const_iterator it;
	for (it = commands.begin(); it != commands.end(); ++it) 
	{
		std::istringstream iss(removeNonPrintable(*it));
		std::string command, param1, param2, param3;
		iss >> command >> param1 >> param2 >> param3;
		//std::cout << "Command: " << command << " Param1: " << param1 << " Param2: " << param2 << " Param3: " << param3 << std::endl;
		if (sender.getIsRegistered() == false)
		{
			//std::cout << "password : " << sender.hasPassword << std::endl;
			//std::cout << "NICK : " << sender.getNick() << std::endl;
			//std::cout << "USER : " << sender.getUser() << std::endl;
			commandsRegister(sender, command, param1);
			if (sender.getHasPassword() == true && sender.getNick().compare(" ") && sender.getUser().compare(" "))
			{
				sender.setIsRegistered(true);
				std::string str = serverReply(SERVER, "001", {sender.getNick()}, "Welcome to ft_irc server!");

				//std::string welcomeMessage = ":ft_irc 001 " + sender.getNick() + " :Welcome to ft_irc server!\r\n";
				//std::cout << welcomeMessage;
				 //sendToClient(str, sender);
			}
		}
		else
		{
			//std::cout << "else" << std::endl;
			std::cout << "Nick: " << sender.getNick()  << std::endl;
			std::cout << "User: " << sender.getUser() << std::endl;
		}
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
//some change
