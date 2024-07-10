#include "Server.hpp"
#include <cctype>
#include <string>

bool g_signal = true;

//#include "parsing_plan.cpp"

//Server.cpp
void Server::initializeReplyMap()
{
	numericReplyMap[001] = {code_001};
	numericReplyMap[002] = {code_002};
	numericReplyMap[003] = {code_003};
	numericReplyMap[331] = {code_331};
	numericReplyMap[332] = {code_332};
	numericReplyMap[353] = {code_353};
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
	numericReplyMap[472] = {code_472};
	numericReplyMap[473] = {code_473};
	numericReplyMap[475] = {code_475};
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
	std::cout << "\nSIGTSTP received. Stopping server..." << g_signal << std::endl;
	g_signal = false;
	std::cout << "SIGTSTP received. Stopping server..." << g_signal << std::endl;
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
		{
			std::cout << _serverFd << std::endl;
			throw(std::runtime_error("failed to bind socket"));
		}

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

	while (g_signal) {
		// Poll sockets
		int pollCount = poll(_fds.data(), _fds.size(), -1);
		if (pollCount < 0) 
		{
			std::cout << "Loop finished" << std::endl;
			close(_serverFd);
			throw(std::runtime_error("poll() failed"));
			//break;
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
	//std::cout << "Loop finished" << std::endl;
	//close(_serverFd);
}

void Server::handleData(int fd, Client &sender, size_t idx)
{
	printf("IN HANDLE DATA\n");
	char buffer[BUFFER_SIZE];
	memset(buffer, 0, BUFFER_SIZE);
	int bytesRead = recv(fd, buffer, BUFFER_SIZE, 0);
	if (bytesRead <= 0) 
	{
		if (bytesRead == 0) 
		{
			std::cout << "Client disconnected (fd: " << fd << ")" << std::endl;
			//close(fd);
			handleQuit(sender);
		}
		else 
			std::cerr << "ERROR reading from socket (fd: " << fd << ")" << std::endl;
		close(fd);
		_fds.erase(_fds.begin() + idx);
		--idx; // Adjust index after erasing
	} 
	else 
	{
		printf("Received message: %s", buffer);
		parseCommand(buffer, sender);
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

void Server::printclient(std::vector<Client> clients)
{
	std::cout << "Print clients" << std::endl;
	int i = 0;
	for(std::vector<Client>::iterator it = clients.begin(); it != clients.end();)
	{
		std::cout << "Client " << i << " " << it->getNick() << std::endl;
		it++;
		i++;
	}
}

void Server::printclientfds(std::vector<Client> clients)
{
	std::cout << "Print clients fds" << std::endl;
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


std::string Server::searchTrailer(const std::string& string, bool flag)
{
	//std::cout << "searchTrailer" << std::endl;
	size_t pos = string.find(':'); 
	if (pos != std::string::npos) 
		return (string.substr(pos + 1));
	else
	{
		pos = string.find_first_of(" ");
		if (pos != std::string::npos) 
		{
			while (pos < string.length() && std::isspace(string[pos])) 
				pos++;
			if (pos < string.length()) 
			{
				while (pos < string.length() && std::isspace(string[pos])) 
					pos++;
				if(!flag)
				{
					std::string newString = searchTrailer(string.substr(pos), true);
					return newString;
				}
				else
					return string.substr(pos);
			} 
		}
		return "";
	}
}

int Server::checkNick(std::string& nick, Client& sender, std::string& param2)
{
	//std::cout << "in checkNick" << std::endl;
	//std::cout << nick << std::endl;
	std::string nickCheck ("#@: $");
	std::string nickCheck2 ("123456789");
	std::string nickCheck3 (",*?!@");
	if(nickCheck.find(nick[0]) != std::string::npos || nickCheck2.find(nick[0]) != std::string::npos || nick.empty() || !param2.empty())
	{
		if (!param2.empty())
			sendToClient(numReplyGenerator(SERVER, {"NICK", nick + " " + param2}, 432), sender);
		else
			sendToClient(numReplyGenerator(SERVER, {"NICK", nick}, 432), sender);
		return 0;
	}
	for(size_t i = 0; i < nick.length(); i++)
	{
		if((!isalnum(nick[i]) && nick[i] != '-' && nick[i] != '_' && nick[i] != '_' && nick.find_first_of("\{}\\\[]|") != std::string::npos) || nickCheck3.find(nick[i]) != std::string::npos)
		{
			sendToClient(numReplyGenerator(SERVER, {"NICK", nick}, 432), sender);
			return 0;
		}
	}
	for (std::vector<Client>::iterator it = _clients.begin(); it != _clients.end(); ++it) {
		if (it->getNick() == nick) 
		{
			std::cout << "Nick is already in use" << std::endl;
			sendToClient(numReplyGenerator(SERVER, {"NICK", nick}, 433), sender);
			return 0;
		}
		//Servers MUST allow at least all alphanumerical characters, square and curly brackets ([]{}), backslashes (\), and pipe (|) characters in nicknames, and MAY disallow digits as the first character
		//no leading # or @  or : or ASCII space character
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

void Server::commandsRegister(Client& sender, std::string& command, std::string& param1, std::string& param2){
	if (command == "NICK")
	{
		if (!checkNick(param1, sender, param2))
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
			sendToClient(numReplyGenerator(SERVER, {"PASS", sender.getNick()}, 464), sender); // we have to handle errors while sending
	}
	else if (command == "QUIT")
		handleQuit(sender);
}


void Server::joinChannel(Client &sender, const std::string& channelName, const std::string& pwd)
{
	std::cout << "Joining channel: " << channelName << std::endl;
	if (channelExists(channelName))
	{
		Channel *channel = returnExistingChannel(channelName);
		if(channel->isPwdProtected() && channel->getKey() != pwd)// not implemented correctly yet
		{
			if(pwd.empty())
				sendToClient(numReplyGenerator(SERVER, {"JOIN", channelName}, 475), sender);
			else
				sendToClient(serverReply(sender.getNick(), "525", {"JOIN", channelName}, "Key is not well-formed"), sender);
			return;
		}
		if (channel->getTopic() == "")
			sendToClient(numReplyGenerator(SERVER, {"JOIN", channelName}, 331), sender);
		else
			sendToClient(serverReply(SERVER, "332", {"TOPIC", channelName}, channel->getTopic()), sender);
		if (channel->containsClient(sender) == true)
		{
			printf("WE ARE HERE!!!!!"); //debug
			std::string errorMessage = numReplyGenerator(SERVER, {"JOIN", sender.getNick(), channelName}, 464);
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
		channel->removeInvite(sender);

		sendToClient(serverReply(sender.getNick(), "JOIN", {channelName}, "Channel joined successfully"), sender);
		broadcastMessage(channel->getClientsVector(), sender, serverReply(sender.getNick(), "JOIN", {channelName}, ""));
	}
	else
	{
		if (isValidChannelName(channelName, sender) == true)
		{
			Channel newChannel(channelName);
			newChannel.addUser(sender);
			newChannel.addOperator(sender);
			_channels.push_back(newChannel);
			sendToClient(serverReply(sender.getNick(), "JOIN", {channelName, sender.getUser()}, ""), sender);
			sendToClient(serverReply(SERVER, "353", listChannelClients(newChannel), ""), sender);
			sendToClient(numReplyGenerator(sender.getNick(), {"JOIN", channelName}, 331), sender);

		}
		else
			sendToClient(serverReply(SERVER, "401", {"JOIN", channelName + ":","Cannot join channel"}, "Channel name must start with a hash mark (#)"), sender);
	}
}

Channel *Server::returnExistingChannel(const std::string& channelName)
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
	std::cout << "in broadcast" << std::endl;
	for (std::vector<Client>::const_iterator it = clients.begin(); it != clients.end(); it++) 
	{
		if((*it).getUser() != sender.getUser())
			sendToClient(message, *it);
	}
	printclient(clients);
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
	list.push_back(clientstr);
		for(size_t i = 0; i < channel.getClientsVector().size(); ++i)
		{
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

Client *Server::findClientByNick(std::string nick)
{
	for (std::vector<Client>::iterator it = _clients.begin(); it != _clients.end(); it++)
	{
		if (it->getNick() == nick)
			return &(*it);
	}
	return NULL;
}


void Server::kickClient (Client &kicker, std::string &channelName, std::string &ejecteeName)
{
	(void)ejecteeName;
	if (!channelExists(channelName))
	{
		sendToClient(numReplyGenerator(kicker.getNick(), {"KICK", channelName}, 403), kicker);
		return;
	}

	Channel *channel = returnExistingChannel(channelName);

	if (channel->clientNotInChannel(kicker))
	{
		sendToClient(numReplyGenerator(kicker.getNick(), {"KICK", channelName}, 442), kicker);
		return;
	}

	if (returnExistingChannel(channelName)->clientNotOperator(kicker))
	{
		sendToClient(numReplyGenerator(kicker.getNick(), {"KICK", channelName}, 482), kicker);
		return;
	}

	if (channel->clientWithThatNameNotInChannel(ejecteeName))
	{
		sendToClient(numReplyGenerator(kicker.getNick(), {"KICK", channelName}, 441), kicker);
		return;
	}
	Client *ejectee = findClientByNick(ejecteeName);
	channel->kick(*ejectee); 
	sendToClient(serverReply(kicker.getNick(), "KICK", {channelName, ejecteeName}, kicker.getNick()), *ejectee);
	broadcastMessage(channel->getClientsVector(), *ejectee, serverReply(kicker.getNick(), "KICK", {channelName, ejecteeName}, kicker.getNick()));
}

void Server::channelMessage(Client &sender, std::string &receiver, std::string &message)
{
	if (!channelExists(receiver))
	{
		sendToClient(numReplyGenerator(sender.getNick(), {"PRIVMSG", receiver}, 403), sender);
		return;
	}
	Channel *channel = returnExistingChannel(receiver);
	if (channel->clientNotInChannel(sender))
	{
		sendToClient(numReplyGenerator(sender.getNick(), {"PRIVMSG", receiver}, 442), sender);
		return;
	}
	std::vector<Client> clients = channel->getClientsVector();
	broadcastMessage(clients, sender, serverReply(sender.getNick(), "PRIVMSG", {receiver}, message));
}

void Server::handlePrivmsg(Client &sender, std::string &receiver, std::string &message)
{
	if(receiver.empty())
	{
		sendToClient(numReplyGenerator(SERVER, {"PRIVMSG"}, 411), sender);
		return;
	}
	if(message.empty())
	{
		sendToClient(numReplyGenerator(SERVER, {"PRIVMSG"}, 412), sender);
		return;
	}
	if (receiver[0] == '#')
	{
		channelMessage(sender, receiver, message);
	}
	else
	{
		Client *receiverClient = findClientByNick(receiver);
		if (!receiverClient)
		{
			sendToClient(numReplyGenerator(sender.getNick(), {"PRIVMSG", receiver}, 401), sender);
			return;
		}
		sendToClient(serverReply(sender.getNick(), "PRIVMSG", {receiver}, message), *receiverClient);
	}
}

void Server::namesChannel(Client& sender, const std::string& channelName)
{
	if (channelExists(channelName))
	{
		std::cout << "nameschanel" << std::endl;
		Channel *channel = returnExistingChannel(channelName);
		sendToClient(serverReply(SERVER, "353", listChannelClients(*channel), ""), sender);
		broadcastMessage(channel->getClientsVector(), sender, serverReply(SERVER, "353", listChannelClients(*channel), ""));
	}
}

void Server::handleNick(Client& sender, std::string& newNick, std::string& param2)
{
	if(!checkNick(newNick, sender, param2))
		return;
	if(sender.getNick() != newNick)
	{
		for(std::vector<Client>::iterator it = _clients.begin(); it != _clients.end(); ++it)
		{
			if((*it).getNick() == newNick)
			{
				sendToClient(numReplyGenerator(SERVER, {"NICK", newNick}, 433), sender);
				return;
			}
		}
		sendToClient(serverReply(sender.getNick(), "NICK", {newNick}, ""), sender);
		for(std::vector<Channel>::iterator it = _channels.begin(); it != _channels.end(); ++it)
			(*it).updateNick(sender, newNick);
		broadcastMessage(_clients, sender, serverReply(sender.getNick(), "NICK", {newNick}, ""));
		sender.setNickName(newNick);
	}
	//sendToClient(serverReply(sender.getNick(), "NICK", {newNick}, ""), sender);
}

void Server::handleQuit(Client& sender)
{
	if(nickIsInServer(sender.getNick()))
	{
		std::cout << "Client " << sender.getNick() << " is quitting." << std::endl;
		for(std::vector<Channel>::iterator it = _channels.begin(); it != _channels.end(); ++it)//Broadcast leave message
		{
			if((*it).clientWithThatNameNotInChannel(sender.getNick()))
				continue;
			broadcastMessage((*it).getClientsVector(), sender, std::string("left the channel ") + (*it).getChannelName());
		}
		removeClientFromChannels(sender);//remove client from Channels & check if channel members is not 0
		removeClientFromServer(sender);//handle client resources closing fd
		printf("Client removed 1\n");
	}
	else 
		std::cerr << "Warning: Client with nickname '" << sender.getNick() << "' not found on server." << std::endl;
	printf("Client removed 2\n");

}

void Server::removeClientFromChannels(Client& client)
{
	for (std::vector<Channel>::iterator it = _channels.begin(); it != _channels.end(); ++it)
	{
		it->removeClient(client);
		if(it->removeOperator(client))//if removeOperator exists (1) new operator assigned due 0 operators left then broadcast to channel all clients
			broadcastMessage(it->getClientsVector(), client, serverReply(SERVER, "353", listChannelClients(*it), ""));
	}
	clearChannelsNoUsers();
}

void	Server::clearChannelsNoUsers()
{
	for (std::vector<Channel>::iterator it = _channels.begin(); it != _channels.end(); ++it) 
	{
		if (it->getUsernum() == 0) 
		{
			it->clearVectors();
			_channels.erase(it);
			break;
		}
	}
}

void Server::removeClientFromServer(Client& client)
{
	std::cout << "Removing Client " << client.getNick() << " from server." << std::endl;
	int i = -1;
	while (++i < (int)_fds.size())
	{
		if (_fds[i].fd == client.getFd())
		{
			close(client.getFd());
			_fds.erase(_fds.begin() + i);
			break;
		}
	}
	i = -1;
	while (++i < (int)_clients.size())
	{
		if (_clients[i].getNick() == client.getNick())
		{
			_clients.erase(_clients.begin() + i);
			break;
		}
	}
	printf("REMOVED CLIENT\n");

}


void Server::mode(std::string channel, std::string mode, std::string parameter, Client& client)
{
	std::cout << "Channel: " << channel << " Mode: " << mode << " Parameter: " << parameter << std::endl;
	if (channel.empty() && mode.empty() && parameter.empty())
		return;
	else if(channelExists(channel) && mode.empty())
	{
		std::string activeModes = "Modes on " + returnExistingChannel(channel)->getChannelName() + " are " + returnExistingChannel(channel)->getModes();
		std::cout << "Active modes: " << activeModes << std::endl;
		sendToClient(serverReply(client.getNick(), "351", {"MODE", channel, activeModes}, ""), client);
    	return;
	}
	else if(channelExists(channel) && returnExistingChannel(channel)->clientNotOperator(client))
	{
		//std::cout << "if not operator how many operators: " << returnExistingChannel(channel)->getOperatorsVector().size() << " " << returnExistingChannel(channel)->clientNotOperator(client) << std::endl;
		printclient(returnExistingChannel(channel)->getOperatorsVector());
		std::cout << "Client is not operator" << std::endl;
		std::string errorMessage = numReplyGenerator(SERVER, {"NOTICE", channel}, 482); 
		return;//may be changed
	}
	if (channelExists(channel) && !mode.empty())
			chooseMode(channel, mode, parameter, client);
	else
	{
			//std::string errorMessage = numReplyGenerator(SERVER, {"NOTICE", channel}, 403); 
			//sendToClient(errorMessage, client);
	}
}

bool Server::clientIsOperator(std::string channelName, Client& client) {
    Channel* modeChannel = returnExistingChannel(channelName);
    // Use the new getOperators() method
		std::cout << "number of operators" << modeChannel->getOperatorsVector().size() << std::endl;
    for (std::vector<Client>::const_iterator it = modeChannel->getOperatorsVector().begin(); it != modeChannel->getOperatorsVector().end(); ++it) {
        if (it->getNick() == client.getNick())
            return true;
    }
    return false;
}

void Server::chooseMode(std::string channel, std::string mode, std::string parameter, Client& client)
{
	if (mode == "+o")
		modeOperator(channel, parameter, client, "positive");
	else if (mode == "-o")
		modeOperator(channel, parameter, client, "negative");
	else if (mode == "+k")
		modeKey(channel, parameter, client, "positive");
	else if (mode == "-k")
		modeKey(channel, parameter, client, "negative");
	else if (mode == "+l")
		modeLimit(channel, parameter, client, "positive");
	else if (mode == "-l")
		modeLimit(channel, parameter, client, "negative");
	else if (mode == "+t")
		modeTopic(channel, parameter, client, "positive");
	else if (mode == "-t")
		modeTopic(channel, parameter, client, "negative");
	else if (mode == "+i")
		modeInvite(channel, parameter, client, "positive");
	else if (mode == "-i")
		modeInvite(channel, parameter, client, "negative");
	else
	{
		std::string errorMessage = numReplyGenerator(SERVER, {"NOTICE", channel}, 472);
		sendToClient(errorMessage, client);
	}
}

void Server::modeOperator(std::string channel, std::string parameter, Client& client, std::string mode)
{
	std::cout << "Parameter: " << parameter << std::endl;
	if (nickIsInServer(parameter) == false)
	{
		std::cout << "Nick is not in server" << std::endl;
		sendToClient(numReplyGenerator(SERVER, {"NOTICE", parameter}, 401), client);
		return;
	}
	Channel *modeChannel = returnExistingChannel(channel);
	if ((!modeChannel->clientNotInChannel(client) && modeChannel->getClient(parameter)) && modeChannel->clientNotOperator(client))
	{
		Client* newOperator = modeChannel->getClient(parameter);
		if (mode == "positive" ) // the sender is not seeing the change and any message
		{
			modeChannel->addOperator(*newOperator);
			std::vector<Client> clients = modeChannel->getClientsVector();
			broadcastMessage(clients, client, serverReply(client.getNick(), "MODE", {channel, "+o", parameter}, ""));
			sendToClient(serverReply(client.getNick(), "MODE", {channel, "+o", parameter}, ""), client);
		}
		else
		{
			modeChannel->removeOperator(*newOperator);
			std::vector<Client> clients = modeChannel->getClientsVector();
			broadcastMessage(clients, client, serverReply(client.getNick(), "MODE", {channel, "-o", parameter}, ""));
			sendToClient(serverReply(client.getNick(), "MODE", {channel, "-o", parameter}, ""), client);
		}
	}
}

bool Server::nickIsInServer(std::string nick)
{
	for (std::vector<Client>::iterator it = _clients.begin(); it != _clients.end(); it++)
	{
		if (it->getNick() == nick)
			return true;
	}
	return false;
}

void Server::modeKey(std::string channel, std::string parameter, Client& client, std::string mode)
{
	Channel *modeChannel = returnExistingChannel(channel);
	if (!modeChannel->clientNotInChannel(client))
	{
		if (mode == "positive" && !parameter.empty())
		{
			modeChannel->addMode('k');
			modeChannel->setKey(parameter);
			modeChannel->setIfHasPassword(true);
			std::vector<Client> clients = modeChannel->getClientsVector();
			broadcastMessage(clients, client, serverReply(client.getNick(), "MODE", {channel, "+k", parameter}, " "));
			sendToClient(serverReply(client.getNick(), "MODE", {channel, "+k", parameter}, " "), client); 
		}
		else if (mode == "negative")
		{
			modeChannel->removeMode('k');
			modeChannel->removeKey();
			modeChannel->setIfHasPassword(false);
			std::vector<Client> clients = modeChannel->getClientsVector();
			broadcastMessage(clients, client, serverReply(client.getNick(), "MODE", {channel, "-k", parameter}, " "));
			sendToClient(serverReply(client.getNick(), "MODE", {channel, "-k", parameter}, " "), client);
		}
	}
}

void Server::modeInvite(std::string channel, std::string parameter, Client& client, std::string mode)
{
	Channel *modeChannel = returnExistingChannel(channel);
	if (!modeChannel->clientNotInChannel(client))
	{
		if (mode == "positive")
		{
			modeChannel->addMode('i');
			modeChannel->setInviteOnly(true);
			std::vector<Client> clients = modeChannel->getClientsVector();
			broadcastMessage(clients, client, serverReply(client.getNick(), "MODE", {channel, "+i", parameter}, " "));//attaching something after the name of the channel
			sendToClient(serverReply(client.getNick(), "MODE", {channel, "+i", parameter}, " "), client); 
		}
		else
		{
			modeChannel->removeMode('i');
			modeChannel->setInviteOnly(false);
			std::vector<Client> clients = modeChannel->getClientsVector();
			broadcastMessage(clients, client, serverReply(client.getNick(), "MODE", {channel, "-i", parameter}, " "));
			sendToClient(serverReply(client.getNick(), "MODE", {channel, "-i", parameter}, " "), client);
		}
	}
}

void Server::modeTopic(std::string channel, std::string parameter, Client& client, std::string mode)
{
	Channel *modeChannel = returnExistingChannel(channel);
	if (!modeChannel->clientNotInChannel(client))
	{
		if (mode == "positive")
		{
			modeChannel->addMode('t');
			modeChannel->setRestrictTopic(true);
			std::vector<Client> clients = modeChannel->getClientsVector();
			broadcastMessage(clients, client, serverReply(client.getNick(), "MODE", {channel, "+t", parameter}, " "));
			sendToClient(serverReply(client.getNick(), "MODE", {channel, "+t", parameter}, " "), client);
		}
		else
		{
			modeChannel->removeMode('t');
			modeChannel->setRestrictTopic(false);
			std::vector<Client> clients = modeChannel->getClientsVector();
			broadcastMessage(clients, client, serverReply(client.getNick(), "MODE", {channel, "-t", parameter}, " "));
			sendToClient(serverReply(client.getNick(), "MODE", {channel, "-t", parameter}, " "), client);
		}
	}
}

bool Server::isStringNumeric(const std::string& str) {
    for (std::string::const_iterator it = str.begin(); it != str.end(); ++it) {
        if (!isdigit(static_cast<unsigned char>(*it))) {
            return false;
        }
    }
    return true;
}
void Server::modeLimit(std::string channel, std::string parameter, Client& client, std::string mode)
{
	Channel *modeChannel = returnExistingChannel(channel);
	if (!modeChannel->clientNotInChannel(client))
	{
		if (mode == "positive" && !parameter.empty() && isStringNumeric(parameter))
		{
			modeChannel->addMode('l');
			modeChannel->setMaxUsers(std::stoi(parameter));
			modeChannel->sethasMaxUsers(true);
			std::vector<Client> clients = modeChannel->getClientsVector();
			broadcastMessage(clients, client, serverReply(SERVER, "MODE", {channel, "+l", parameter}, ""));
			sendToClient(serverReply(client.getNick(), "MODE", {channel, "+l", parameter}, ""), client);
		}
		else if (mode == "negative")
		{
			modeChannel->removeMode('l');
			modeChannel->setInviteOnly(false);
			modeChannel->sethasMaxUsers(false);
			std::vector<Client> clients = modeChannel->getClientsVector();
			broadcastMessage(clients, client, serverReply(SERVER, "MODE", {channel, "-l", parameter}, ""));
			sendToClient(serverReply(client.getNick(), "MODE", {channel, "-l", parameter}, ""), client);
			
		}
	}
}


void Server::part(Client& sender, std::string &channelName, std::string &trailer)
{
	Channel *channel = returnExistingChannel(channelName);
	if (channel)
	{
		if (channel->clientNotInChannel(sender))
		{
			sendToClient(numReplyGenerator(sender.getNick(), {"PART", channelName}, 442), sender);
			return;
		}
		if (trailer.empty())
			trailer = "Leaving channel";
		sendToClient(serverReply(sender.getNick(), "PART", {channelName}, trailer), sender);
		broadcastMessage(channel->getClientsVector(), sender, serverReply(sender.getNick(), "PART", {channelName}, trailer));
		channel->removeUser(sender);
		if (channel->getUsernum() == 0)
		{
			printf("Channel is empty, removing it in PART");
			std::vector<Channel>::iterator it = _channels.begin();
			while (it != _channels.end())
			{
				if (it->getChannelName() == channelName)
				{
					_channels.erase(it);
					break;
				}
				++it;
			}
		}
	}
	else
		sendToClient(numReplyGenerator(sender.getNick(), {"PART", channelName}, 403), sender);
}


//The INVITE command is used to invite a user to a channel. The parameter <nickname> is the nickname of the person to be invited to the target channel <channel>.
//
//The target channel SHOULD exist (at least one user is on it). Otherwise, the server SHOULD reject the command with the ERR_NOSUCHCHANNEL numeric.
//
//Only members of the channel are allowed to invite other users. Otherwise, the server MUST reject the command with the ERR_NOTONCHANNEL numeric.
//
//Servers MAY reject the command with the ERR_CHANOPRIVSNEEDED numeric. In particular, they SHOULD reject it when the channel has invite-only mode set, and the user is not a channel operator.
//
//If the user is already on the target channel, the server MUST reject the command with the ERR_USERONCHANNEL numeric.
//
//When the invite is successful, the server MUST send a RPL_INVITING numeric to the command issuer, and an INVITE message, with the issuer as <source>, to the target user. Other channel members SHOULD NOT be notified.


bool Server::clientExists(std::string &nick)
{
	for (std::vector<Client>::iterator it = _clients.begin(); it != _clients.end(); it++)
	{
		if (it->getNick() == nick)
			return true;
	}
	return false;
}


void Server::inviteToChannel(Client &sender, std::string &invitee, std::string &channel)
{
	if (clientExists(invitee) == false)
	{
		sendToClient(numReplyGenerator(sender.getNick(), {"INVITE", channel}, 401), sender);
		return;
	}
	if (channelExists(channel) == false)
	{
		sendToClient(numReplyGenerator(sender.getNick(), {"INVITE", channel}, 403), sender);
		return;
	}
	Channel *inviteChannel = returnExistingChannel(channel);
	if (inviteChannel->clientNotInChannel(sender))
	{
		sendToClient(numReplyGenerator(sender.getNick(), {"INVITE", channel}, 442), sender);
		return;
	}
	if (inviteChannel->isInviteOnly() && inviteChannel->clientNotOperator(sender))
	{
		sendToClient(numReplyGenerator(sender.getNick(), {"INVITE", channel}, 482), sender);
		return;
	}
	Client *inviteeClient = findClientByNick(invitee);
	if (!inviteChannel->clientNotInChannel(*inviteeClient))
	{
		sendToClient(numReplyGenerator(sender.getNick(), {"INVITE", channel}, 443), sender);
		return;
	}
	sendToClient(serverReply(sender.getNick(), "INVITE", {invitee, channel}, ""), sender);
	sendToClient(serverReply(sender.getNick(), "INVITE", {invitee, channel}, ""), *inviteeClient);
	inviteChannel->invite(*inviteeClient);
}
bool Server::isValidChannelName(const std::string& name, Client &sender) {
	if (name.empty())
		return false;
    if (name[0] != '#')
	{
		sendToClient(numReplyGenerator(SERVER, {"MODE", sender.getNick()}, 002), sender);
		return false;
	}
    if (name.length() > 50)
	{
		sendToClient(numReplyGenerator(SERVER, {"MODE", sender.getNick()}, 003), sender);
		return false;
	}
    for (std::string::const_iterator it = name.begin(); it != name.end(); ++it) {
        if (*it == ' ' || *it == '\x07' || *it == ',')
		{
			sendToClient(numReplyGenerator(SERVER, {"MODE", sender.getNick()}, 002), sender);
			return false;
		}
    }
    return true;
}
void Server::commandsAll(Client &sender, std::string &command, std::string &parameter1, std::string &parameter2, std::string &parameter3, std::string &trailer)
{
	if (command == "JOIN")
		joinChannel(sender, parameter1, parameter2);
	else if (command == "PART" )
		part(sender, parameter1, trailer);
	else if (command == "TOPIC")
		channelTopic(sender, parameter1, trailer);
	else if (command == "PRIVMSG")
		handlePrivmsg(sender, parameter1, trailer);
	else if (command == "WHO")
		namesChannel(sender, parameter1);
	else if (command == "KICK")
		kickClient(sender, parameter1, parameter2);
	else if (command == "INVITE")
		inviteToChannel(sender, parameter1, parameter2);
	else if (command == "MODE" && isValidChannelName(parameter1, sender))
			mode(parameter1, parameter2, parameter3, sender);
	else if (command == "USER") 
		sendToClient(numReplyGenerator(SERVER, {"USER", sender.getNick()}, 462), sender);
	else if (command == "NICK")
		handleNick(sender, parameter1, parameter2);
	else if (command == "QUIT") 
		handleQuit(sender);
}

void Server::parseCommand(std::string clientData, Client& sender){
	//std::cout << "Current client : " << sender.getFd() << std::endl;
	std::vector<std::string> commands = splitString(clientData, "\r\n");
	std::vector<std::string>::const_iterator it;
	for (it = commands.begin(); it != commands.end(); ++it) 
	{
		std::istringstream iss(removeNonPrintable(*it));
		std::string command, param1, param2, param3, trailer;
		trailer = searchTrailer(iss.str(), false);
		iss >> command >> param1 >> param2 >> param3;
		//std::cout << "Command: " << command << " Param1: " << param1 << " Param2: " << param2 << " Trailer: " << trailer << std::endl;
		std::string upperCase = uppercasify(command);		
		if (sender.getIsRegistered() == false)
		{
			commandsRegister(sender, upperCase, param1, param2);
			if (sender.getHasPassword() == true && sender.getNick().compare("") && sender.getUser().compare(""))
			{
				sender.setIsRegistered(true);
				sendToClient(serverReply(SERVER, "001", {sender.getNick()}, "Welcome to ft_irc server!"), sender);
			}
			//else
			//sendToClient(serverReply(SERVER, "001", {sender.getNick()}, "Please complete your registration"), sender);
		}
		else
		{	
			if (isValidCommand(upperCase) == false && !upperCase.empty())
				sendToClient(numReplyGenerator(sender.getNick(), {""}, 421), sender);
			else
			{
				//std::cout << "COMMAND: " << upperCase << std::endl;
				//std::cout << "PARAM1: " << param1 << std::endl;
				//std::cout << "PARAM2: " << param2 << std::endl;
				//std::cout << "PARAM3: " << param3 << std::endl;
				//std::cout << "TRAILER: " << trailer << std::endl;
				commandsAll(sender, upperCase, param1, param2, param3, trailer);
			}
		}
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
    //result.push_back(str);
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

std::string Server::uppercasify(std::string& str)
{
	std::string result(str); 
	for (size_t i = 0; i < str.length(); ++i) 
		result[i] = toupper(str[i]); 
	return result;
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
