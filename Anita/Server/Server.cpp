#include "Server.hpp"

#include "parsing_plan.cpp"
#include <string>

//Server.cpp
void Server::initializeReplyMap()
{
	numericReplyMap[331] = {code_331};
	numericReplyMap[366] = {code_366};
	numericReplyMap[401] = {code_401};
	numericReplyMap[403] = {code_403};
	numericReplyMap[404] = {code_404};
	numericReplyMap[411] = {code_411};
	numericReplyMap[412] = {code_412};
	numericReplyMap[421] = {code_421};
	numericReplyMap[431] = {code_431};
	numericReplyMap[432] = {code_432};
	numericReplyMap[433] = {code_433};
	numericReplyMap[441] = {code_441};
	numericReplyMap[442] = {code_442};
	numericReplyMap[443] = {code_443};
	numericReplyMap[461] = {code_461};
	numericReplyMap[462] = {code_462};
	numericReplyMap[464] = {code_464};
	numericReplyMap[471] = {code_471};
	numericReplyMap[473] = {code_473};
	numericReplyMap[482] = {code_482};
}

Server::Server()
{
}

Server::Server(int port, std::string password) : _serverFd(-1), _port(port), _pwd(password)
{
	_myValidCommands = listValidCommands();
	initializeReplyMap();
}

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





		//buffer[bytesRead] = '\r';
		//	ircMessageParser(buffer, *this);
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

std::string Server::numReplyGenerator(const std::string& client, const std::vector<std::string>& params, int errorCode)
{
	if(numericReplyMap.find(errorCode) != numericReplyMap.end())
	{

		std::string intStr;
		std::stringstream cmd;
		//std::cout << "Error code: " << errorCode << std::endl;
		cmd << errorCode;

		cmd >> intStr;
		//std::cout << "intStr: " << intStr << std::endl;
		if(params.empty())
			return (serverReply(client, intStr, {""}, numericReplyMap[errorCode]));

		return (serverReply(client, intStr, params, numericReplyMap[errorCode]));
	}
	return ("No error Code found in numReplyGenerator");
}

std::string Server::serverReply(const std::string& prefix, const std::string& cmd, const std::vector<std::string>& params, const std::string& trailingParam)
{
	//std::cout << "in serverReply" << std::endl;
	std::stringstream message;
	// Build the message components
	message << ":" << prefix << " " << cmd;
	// Add parameters if any
	if (!params.empty()) 
	{
		for(size_t i = 0; i < params.size(); i++)
		{
			if (params[i] == "")
				message << " " << "*";
			else
				message << " " << params[i];
		}
	}
	else
		message << " " << "*";

	// Add trailing parameter if provided
	if (!trailingParam.empty()) {
		message << " :" << trailingParam;
	}
	message << "\r\n";
	// Return the complete server reply string
	return message.str();
		//e.g. 	std::string str = serverReply(SERVER, "001", {"exampleNick"}, "Welcome to ft_irc serverrrr!");
		//":ft_irc 001 exampleNick :Welcome to ft_irc server!\n";
		//":clientNick JOIN #thisChannel ("NULL");
		//:ircserver.com 433 *  Nickname is already in use;

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


std::string Server::searchTrailer(const std::string& string)
{
		size_t colonPos = string.find(':'); 
		if (colonPos != std::string::npos) 
			return (string.substr(colonPos + 1));
		else
			return "";
}

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
		//std::cout << "NICK : " << sender.getNick() << std::endl;
		if (param1 == _pwd)
			sender.setHasPassword(true);
		else
		{
			//std::cout << "Password is incorrect" << std::endl;
			std::string errorMessage = numReplyGenerator(SERVER, {"PASS", sender.getNick()}, 464);
			//std::cout << "test 1" << std::endl;
			sendToClient(errorMessage, sender); // we have to handle errors while sending
			//std::cout << "Password is incorrect test2" << std::endl;
		}
	}
	else if (command == "QUIT")
		{
			close(sender.getFd());

		}
}

void Server::joinChannel(Client &sender, std::string channelName)
{
	std::cout << "Joining channel: " << channelName << std::endl;
	if (channelExists(channelName))
	{
		Channel *channel = returnExistingChannel(channelName);
		if (channel->getTopic() == "")
			sendToClient(numReplyGenerator(SERVER, {"JOIN", channelName}, 331), sender);
		else
			sendToClient(serverReply(SERVER, "332", {"TOPIC", channelName}, channel->getTopic()), sender);
		if (channel->containsClient(sender) == true)
		{
			std::string errorMessage = numReplyGenerator(SERVER, {"JOIN", sender.getNick(), channelName}, 464);
			//std::string errorMessage = serverReply(SERVER, "403", {sender.getNick(), channelName}, "You are already in that channel");
			sendToClient(errorMessage, sender);
			return;
		}
		if (channel->isInviteOnly() && channel->isInvitedToChannel(sender) == false)
		{
			std::string errorMessage = serverReply(SERVER, "473", {sender.getNick(), channelName}, "Channel is invite only, and you are not invited");
			sendToClient(errorMessage, sender);
			return;
		}
		if (channel->hasMaxUsers() && channel->getUsernum() >= channel->maxNumOfUsers())
		{
			std::string errorMessage = serverReply(SERVER, "471", {sender.getNick(), channelName}, "Channel is full");
			sendToClient(errorMessage, sender);
			return;
		}
		channel->addUser(sender);
		std::string successMessage = serverReply(sender.getNick(), "JOIN", {channelName}, "Channel joined successfully");
		sendToClient(successMessage, sender);
		sendToClient(serverReply(SERVER, "353", listChannelClients(*channel), ""), sender);
		broadcastMessage(channel->getClientsVector(), sender, serverReply(SERVER, "353", listChannelClients(*channel), ""));

	}
	else
	{
		Channel newChannel(channelName);
		newChannel.addUser(sender);
		newChannel.addOperator(sender);
		_channels.push_back(newChannel);
		std::string successMessage = serverReply(sender.getNick(), "JOIN", {channelName}, ""); sendToClient(successMessage, sender);
		//sendToClient(serverReply(SERVER, "353", {channelName, "=", channelName, "" + sender.getNick().insert(0, "@")}, ""), sender);
		sendToClient(serverReply(SERVER, "353", listChannelClients(newChannel), ""), sender);
		sendToClient(numReplyGenerator(sender.getNick(), {"JOIN", channelName}, 331), sender);
	}
}

Channel *Server::returnExistingChannel(std::string &channelName)
{
	for (std::vector<Channel>::iterator it = _channels.begin(); it != _channels.end(); it++)
	{
		if (it->getChannelName() == channelName)
			return &(*it);
	}
	return NULL;
} 

void Server::broadcastMessage(const std::vector<Client>& clients, Client& sender, const std::string& message)
{
	for (std::vector<Client>::const_iterator it = clients.begin(); it != clients.end(); it++) 
	{
		if((*it).getUser() != sender.getUser())
			sendToClient(message, *it);
	}
}

void Server::channelTopic(Client &sender, std::string channelName, std::string trailer)
{
	Channel* topicChannel = returnExistingChannel(channelName);
	if (channelName.find('#') != std::string::npos && topicChannel)
	{
		if (topicChannel->clientNotInChannel(sender))
		{
			sendToClient(numReplyGenerator(sender.getNick(), {"TOPIC", channelName}, 442), sender); return;
		}
		if (topicChannel->clientNotOperator(sender) && topicChannel->isrestrictTopic())//CHECK THAT CREATOR OF CHANNEL BECOMES OPERATOR
		{
			//std::cout << "CHANNEL restrictTopic: " << topicChannel->isrestrictTopic() << std::endl;
			sendToClient(numReplyGenerator(sender.getNick(), {"TOPIC", channelName}, 482), sender); return;
		}
		
		if (trailer.empty() && !topicChannel->getTopic().empty())//CALLING TO SHOW CURRENT TOPIC
			sendToClient(serverReply(SERVER, "332", {"TOPIC", channelName}, topicChannel->getTopic()), sender);
		else if (!trailer.empty())//CHANGING THE TOPIC
		{
			topicChannel->setTopic(trailer);
			//TOPIC CHANGED, SEND MESSAGE TO ALL CHANNEL_MEMBERS
			sendToClient(serverReply(SERVER, "332", {"TOPIC", channelName}, topicChannel->getTopic()), sender);
			std::vector<Client> clients = topicChannel->getClientsVector();
			broadcastMessage(clients, sender, serverReply(SERVER, "333" ,{"TOPIC", channelName }, sender.getNick()));
			broadcastMessage(clients, sender, serverReply(SERVER, "332", {"TOPIC", channelName}, topicChannel->getTopic()));
		}
		else
			sendToClient(numReplyGenerator(sender.getNick(), {"TOPIC", channelName}, 331), sender);//TOPIC IS NOT SET
	}
	else
		sendToClient(numReplyGenerator(sender.getNick(), {"TOPIC"}, 403), sender);
}
	

std::vector<std::string> Server::listChannelClients(Channel& channel)
{
	std::vector<std::string> list;
	std::string clientstr = channel.getChannelName() + " = " + channel.getChannelName();
	size_t j;
	list.push_back(clientstr);
		for(size_t i = 0; i < channel.getClientsVector().size(); ++i)
		{
			j = 0;
			clientstr = channel.getClientsVector()[i].getNick();
			//std::cout << clientstr << std::endl;
			for(size_t j = 0; j < channel.getOperatorsVector().size(); ++j)
			{
				//std::cout << channel.getOperatorsVector()[j].getNick() << std::endl;
				if(channel.getOperatorsVector()[j].getNick() == channel.getClientsVector()[i].getNick())
					clientstr.insert(0, "@");
				//std::cout << clientstr << std::endl;
			}
			list.push_back(clientstr);
		}
		//std::cout << "out if lkoop" << std::endl;
	return list;
}

void Server::commandsAll(Client sender, std::string command, std::string parameter1, std::string parameter2, std::string trailer)
{
	(void)parameter2;
	(void)trailer;
	if (command == "JOIN")
	{
		joinChannel(sender, parameter1);
		std::cout << "JOIN" << std::endl;
	}
	// else if (command == "PART")
	// 	//part();
	else if (command == "TOPIC")
		channelTopic(sender, parameter1, trailer);
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
	
}

void Server::parseCommand(std::string clientData, Client& sender){
	//std::cout << "Current client : " << sender.getFd() << std::endl;
	std::vector<std::string> commands = splitString(clientData, "\r\n");
	std::vector<std::string>::const_iterator it;
	for (it = commands.begin(); it != commands.end(); ++it) 
	{
		std::istringstream iss(removeNonPrintable(*it));
		std::string command, param1, param2, trailer;
		trailer = searchTrailer(iss.str());
		iss >> command >> param1 >> param2;
		//std::cout << "Command: " << command << " Param1: " << param1 << " Param2: " << param2 << " Trailer: " << trailer << std::endl;
		if (sender.getIsRegistered() == false)
		{
			commandsRegister(sender, command, param1);
			if (sender.getHasPassword() == true && sender.getNick().compare("") && sender.getUser().compare(""))
			{
				sender.setIsRegistered(true);
			//	std::cout << "NICK : " << sender.getNick() << std::endl;
				std::string str = serverReply(SERVER, "001", {sender.getNick()}, "Welcome to ft_irc server!");
				sendToClient(str, sender);
			}
		}
		else
		{	
			if (isValidCommand(command) == false && !command.empty())
			{
				std::string errorMessage = numReplyGenerator(sender.getNick(), {""}, 421);
				sendToClient(errorMessage, sender);
			}
			else
			{
				commandsAll(sender, command, param1, param2, trailer);
			}
			
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


bool Server::isValidCommand(const std::string& inputCommand) {
	std::vector<std::string>::const_iterator it;
	for (it = _myValidCommands.begin(); it != _myValidCommands.end(); ++it) {
		if (*it == inputCommand)
			return true;
	}
	return false;
}
std::vector<std::string> Server::listValidCommands()
{
	//std::cout << "List of valid commands" << std::endl;
 	_myValidCommands.push_back("JOIN");
 	_myValidCommands.push_back("PART");
 	_myValidCommands.push_back("PRIVMSG");
 	_myValidCommands.push_back("QUIT");
 	_myValidCommands.push_back("NICK");
 	_myValidCommands.push_back("USER");
 	_myValidCommands.push_back("KICK");
 	_myValidCommands.push_back("MODE");
 	_myValidCommands.push_back("INVITE");
 	_myValidCommands.push_back("TOPIC");
 	_myValidCommands.push_back("PASS");
 	_myValidCommands.push_back("NOTICE");
 	_myValidCommands.push_back("WHO");
	//std::cout << "List of valid commands end" << std::endl;
	return (_myValidCommands);
}


