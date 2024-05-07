#ifndef CLIENT_HPP
#define CLIENT_HPP
#include <string>

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
};

#endif // CLIENT_HPP!
