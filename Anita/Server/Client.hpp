#ifndef CLIENT_HPP
# define CLIENT_HPP
#include <string>
#include <sys/socket.h>
#include <iostream>
#include <ostream>
class Client 
{
	private:
		// Add your private members here
		int 				_fd;
		std::string _nick;
		std::string _user;
		bool _isRegistered;
		//bool _hasPassword;
		//	std::string _operatorPassword;
		//	std::string _operatorNickname;
		//	std::string _operatorUsername;
		//	std::string _password;
		//	std::string _username;
		//	std::string _nickname;

		//	bool _isoperator;
		//	bool _registrationcomplete;

	public:
		Client(void);
		~Client(void);

		
		// GETTERS UND SETTERS
		int getFd() const; 
		void setFd(int fd);
		//	void setPassword(std::string& password);
		void setNickName(std::string& nick);
		void setUserName(std::string& user);
		void setIsRegistered(bool status);
		void setHasPassword(bool status);
		//	std::string getPassword() const;
		//	std::string getNickname() const;
		//	std::string getUsername() const;
		//	void setOperator(bool status);
		std::string getNick() const;
		std::string getUser() const;
		bool getIsRegistered() const;
		bool getHasPassword() const;
		void sendMessage(const std::string& message); //function to send to

		//CONNECTING TO SERVER
		//	bool connectServer();
		//	void sendMessage(const std::string& message);
		//	void receiveMessage();

		//operator commands
		// KICK
		// INVITE
		// TOPIC
		// MODE: i = Set/ remove Invite-only channel ; 

		bool operator==(const Client& other) const;

};

#endif // CLIENT_HPP!
