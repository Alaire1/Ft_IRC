#ifndef CLIENT_HPP
#define CLIENT_HPP
#include <string>

class Client 
{
	private:
		// Add your private members here
		int 				_fd;
		std::string _ipAdd;

		std::string _operatorPassword;
		std::string _operatorNickname;
		std::string _operatorUsername;
		std::string _password;
		std::string _username;
		std::string _nickname;

		bool _isoperator;
		bool _registrationcomplete;

	public:
		Client(void);
		~Client(void);

		// GETTERS UND SETTERS
		int getFd() const; 
		void setFd(int fd);
		void setIpAdd(std::string ipadd);
		void setPassword(std::string& password);
		void setNickname(std::string& nickName);
		void setUsername(std::string& userName);
		std::string getPassword() const;
		std::string getNickname() const;
		std::string getUsername() const;
		void setOperator(bool status);

		//operator commands
		// KICK
		// INVITE
		// TOPIC
		// MODE: i = Set/ remove Invite-only channel ; 
};

#endif // CLIENT_HPP!
