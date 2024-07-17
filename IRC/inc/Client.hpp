#ifndef CLIENT_HPP
# define CLIENT_HPP
#include <string>
#include <sys/socket.h>
#include <iostream>

class Client 
{
	private:
		int 				_fd;
		std::string _nick;
		std::string _user;
		bool _isRegistered;
		bool _hasPassword;

	public:
		Client(void);
		~Client(void);

		
		// GETTERS UND SETTERS
		int getFd() const; 
		void setFd(int fd);
		void setNickName(std::string& nick);
		void setUserName(std::string& user);
		void setIsRegistered(bool status);
		void setHasPassword(bool status);
		std::string getNick() const;
		std::string getUser() const;
		bool getIsRegistered() const;
		std::string getUserName() const;
		bool getHasPassword() const;
		void sendMessage(const std::string& message); //function to send to
		bool operator==(const Client& other) const;

};

#endif // CLIENT_HPP!
