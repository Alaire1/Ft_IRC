#ifndef CLIENT_HPP
# define CLIENT_HPP
#include <string>
#include <sys/socket.h>
#include <iostream>

class Client 
{
	private:
		// Add your private members here
		std::string _ipAdd;
		std::string	_port;
		int 					_fd;
		std::string 	_nick;
		std::string 	_user;

		//	std::string _operatorPassword;
		//	std::string _operatorNickname;
		//	std::string _operatorUsername;
		//	std::string _password;
		//	std::string _username;
		//	std::string _nickname;

		//	bool _isoperator;
			

	public:
		Client(void);
		~Client(void);

		bool isRegistered ;
		bool hasPassword ;
		// GETTERS UND SETTERS
		int getFd() const; 
		void setFd(int fd);
		void setIpAdd(std::string ipadd);
		//	void setPassword(std::string& password);
		void setNickname(std::string& nick);
		void setUsername(std::string& user);
		//	std::string getPassword() const;
		//	std::string getNickname() const;
		//	std::string getUsername() const;
		//	void setOperator(bool status);
		std::string getNick() const;
		std::string getUser() const;
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
};

#endif // CLIENT_HPP!
