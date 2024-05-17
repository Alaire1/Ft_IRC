#ifndef CLIENT_HPP
#define CLIENT_HPP
#include <vector> //-> for vector
#include <sys/socket.h> //-> for socket()
#include <sys/types.h> //-> for socket()
#include <netinet/in.h> //-> for sockaddr_in
#include <fcntl.h> //-> for fcntl()
#include <unistd.h> //-> for close()
#include <arpa/inet.h> //-> for inet_ntoa()
#include <string>
#include <stdbool.h>

class Client 
{
private:
    // Add your private members here
		int _fd;
		std::string _ipAdd;

public:
    Client(void);
    ~Client(void);

    // GETTERS UND SETTERS
		int getFd() const; 
		void setFd(int fd);
		void setIpAdd(std::string ipadd);

		//connect to server
		bool connectServer();
 		void sendMessage(std::string& message);
		void receiveMessage();
};

#endif // CLIENT_HPP!
