#ifndef SERVER_HPP
#define SERVER_HPP
#include <vector> //-> for vector
#include <sys/socket.h> //-> for socket()
#include <sys/types.h> //-> for socket()
#include <netinet/in.h> //-> for sockaddr_in
#include <fcntl.h> //-> for fcntl()
#include <unistd.h> //-> for close()
#include <arpa/inet.h> //-> for inet_ntoa()
#include <poll.h> //-> for poll()
#include <csignal> //-> for signal()
#include <string.h>
#include "Client.hpp"

class Client;
class Server 
{
	private:
		int _port;
		int _serSocketFd;

		static bool signal;
		std::vector<Client> clients;
		std::vector<struct  pollfd> fds; 
		//clients;


	public:
		Server(void);
		~Server(void);

		// Add your class members and methods here
		void serverInit();
		void serSocket();
		void acceptNewClient();
		void receiveNewData(int fd);

		static void SignalHandler(int signum);

		void closeFds();
		void clearClients(int fd);
};

#endif // SERVER_HPP!
