#ifndef SERVER_HPP
#define SERVER_HPP
#include <iostream>
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
#include "Channel.hpp"


#include <iomanip>



#define MAX_CLIENTS 10
#define BUFFER_SIZE 1024
#include "Client.hpp"


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


	public:

		//Orthodox canonical begin
		Server(void);
		Server(int port, std::string password);
		~Server(void);
		//Orthodox canonical end
		static void	signalHandler(int signum);

		// Add your class members and methods here
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
		void commandsAll(Client sender, std::string command, std::string parameter1, std::string parameter2, std::string parameter3);

};
std::string removeNonPrintable(const std::string& input);

// Function to split a string by a given delimiter into a vector of strings
std::vector<std::string> splitString(std::string str, std::string delimiter);


#endif // SERVER_HPP


