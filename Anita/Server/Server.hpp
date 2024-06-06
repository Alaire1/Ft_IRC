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
		//std::vector<Channel> _channels;


public:
    //Server(const std::string& arg); // In case you need to pass an  argument
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
		void handleData(int fd, size_t idx);
		void closeFds();

//TESTING FUNCTIONS
		void    printPassword();
		void 		printclientfds(std::vector<struct Client> clients);
		void    printfds(std::vector<struct pollfd> fds);
		bool    isSocketClosed(int sockfd); 
};


#endif // SERVER_HPP

