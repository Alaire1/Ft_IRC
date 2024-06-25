#ifndef SERVER_HPP
#define SERVER_HPP
#include <iostream>
#include <sstream>
#include <sys/poll.h>
#include <vector>
#include <cstring>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <poll.h>
#include <fcntl.h>
#include <signal.h>
#include <string>
#include <map>
#include <cstddef>
#include <sstream>
//#include <iomanip>
#include "Channel.hpp"
#include "Client.hpp"
#define MAX_CLIENTS 	10
#define BUFFER_SIZE 	1024
#define SERVER 				"ourSuperServer"
#define RESET	"\033[0m"
#define YELLOW	"\033[33m"
#define BLUE    "\033[34m"
#define MAGENTA "\033[35m"
#define RED     "\033[31m"


class Server 
{
	private:
		// Add your private members here
		int _serverFd; 
		int _port;
		std::string _pwd;
		std::vector<struct pollfd> _fds;
		std::vector<Client> _clients;
		std::vector<Channel> _channels;
		std::vector<std::string> _myValidCommands;
		//
		//struct replyMessage {std::string& message;};
		std::map<int, std::string>  numericReplyMap;//numericReplies Map
		void initializeReplyMap();


	public:

		//Orthodox canonical begin
		Server(void);
		Server(int port, std::string password);
		~Server(void);
		//Orthodox canonical end
		static void	signalHandler(int signum);

		// Server member functions
		int  getSocket() const;
		void setNonBlocking(int fd);
		void createSocket();
		void runServer();
		void initServer();
		void acceptClient();
		void handleData(int fd, Client &sender, size_t idx);
		void closeFds();
		int	 clearChannelsNoUsers();//Clears channels with no users inside.

		//TESTING FUNCTIONS
		void    printPassword();
		void 		printclientfds(std::vector<Client> clients);
		void    printfds(std::vector<pollfd> fds);
		bool    isSocketClosed(int sockfd); 

		//PARSING FUNCTIONS
		Client findClientByFd(int fd);
		void parseCommand(std::string clientData, Client &sender);
		int checkNick(std::string nick);
		void commandsRegister(Client &sender, std::string command, std::string param1);
		void commandsAll(Client sender, std::string command, std::string parameter1, std::string parameter2, std::string parameter3, std::string trailer);
		int sendToClient(const std::string& message, const Client& client) const;
		void deleteClient(int fd);
		std::vector<std::string> listValidCommands();
		bool isValidCommand(const std::string& inputCommand);
		std::string searchTrailer(const std::string& string);
		std::string numReplyGenerator(const std::string& client, const std::vector<std::string>& params, int errorCode);
		std::string serverReply(const std::string& prefix, const std::string& cmd, const std::vector<std::string>& params, const std::string& trailingParam);

		//CHANNEL FUNCTIONS
		void joinChannel(Client &sender, std::string channelName);
		Channel *returnExistingChannel(std::string &channelName);
		void broadcastMessage(const std::vector<Client>& clients, Client& sender, const std::string& message);


		//JOIN FUNCTIONS
		void channelTopic(Client &sender, std::string channelName, std::string trailer);
		std::vector<std::string> listChannelClients(Channel& channel);

		//MODE COMMAND FUNCTIONS
		void mode(std::string channelName, std::string mode, std::string parameter, Client &sender);
		bool modeExist(std::string mode);
		bool channelExists(std::string channelName);
		bool clientIsOperator(std::string channelName, Client& client);
		void chooseMode(std::string channel, std::string mode, std::string parameter, Client &sender);
		void modeOperator(std::string channel, std::string parameter, Client& client, std::string mode);
		bool nickIsInServer(std::string nick);
		void modeKey(std::string channel, std::string parameter, Client &sender, std::string mode);
		void modeInvite(std::string channel, std::string parameter, Client &sender, std::string mode);
		void modeTopic(std::string channel, std::string parameter, Client &sender, std::string mode);
		void modeLimit(std::string channel, std::string parameter, Client &sender, std::string mode);


};
//Static strings numeric replies 
static std::string code_331 = "No topic is set";
static std::string code_332 = "Topic is";
static std::string code_366 = "End of /NAMES list";
static std::string code_401 = "No such nick/channel";
static std::string code_403 = "No such channel";
static std::string code_404 = "Cannot send to channel";
static std::string code_411 = "No recipient given PRIVMSG";
static std::string code_412 = "No text to send";
static std::string code_421 = "Unknown command";
static std::string code_431 = "No nickname given";
static std::string code_432 = "Erroneus nickname";
static std::string code_433 = "Nickname is already in use";
static std::string code_441 = "They aren't on that channel";
static std::string code_442 = "You're not on that channel";
static std::string code_443 = "is already on channel";
static std::string code_461 = "Not enough parameters";
static std::string code_462 = "You may not reregister";
static std::string code_464 = "Password incorrect";
static std::string code_471 = "Cannot join channel (+l)";
static std::string code_473 = "Cannot join channel (+i)";
static std::string code_482 = "You're not a channel operator";
std::string removeNonPrintable(const std::string& input);

// Function to split a string by a given delimiter into a vector of strings
std::vector<std::string> splitString(std::string str, std::string delimiter);

#endif // SERVER_HPP

