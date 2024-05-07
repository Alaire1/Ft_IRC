#ifndef SERVER_HPP
#define SERVER_HPP
#include <vector>
#include <sys/socket.h>
#include <sys/types.h>
#include <string>
#include <unistd.h>
#include <poll.h>
#include <fcntl.h>

class Client;
class Server 
{
	private:
		int _port;
		int _serSocketFd;

		static bool signal;
		std::vector<Client> clients;
		std::vector<struct  pollfd> fds; clients;


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
