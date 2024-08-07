#ifndef CHANNEL_HPP
# define CHANNEL_HPP
# include <vector>
# include <algorithm>
# include <string>
# include "Client.hpp"

class Channel
{
	private:
		std::string _name;
		std::string _topic; 
		std::string _key; // password
		std::string _modes;

		bool _isInviteOnly;
		std::vector<Client> _invitees;

		bool _hasMaxUsers;
		int _maxUsers;

		bool _restrictTopic; 
		bool _pwdProtected; 

		std::vector<Client> _clients;
		std::vector<Client> _operators;

		std::string _operatorNick;

	public:
		//Channel();
		Channel(std::string name);
		~Channel();
		void kick(Client& ejectee); 
		void leave(Client& client);
		void invite(Client& client);

		//added by Nikola
		bool clientNotInChannel(Client& client);
		bool isInvitedToChannel(Client& client);
		bool clientNotOperator(Client& client);
		void removeInvite(Client& client);
		bool clientWithThatNameNotInChannel(std::string name);
		int  checkIfOperatorleft();
		void setIfHasPassword(bool);
		//NICK
		void updateNick(Client& client, std::string& newNick);
		//QUIT
		int  removeOperator(Client& client);
		void removeClient(Client& client);
		void clearVectors();

		//setters
		void setInviteOnly(bool isInviteOnly);
		void setRestrictTopic(bool restrictTopic);
		void setMaxUsers(int maxUsers);
		void sethasMaxUsers(bool setter);

		void setOperator(Client& client);
		void setTopic(std::string topic);
		void setKey(std::string key);
		void topicCommand(Client& client, std::string topic);
		void removeKey(); //MODE -k
		bool containsClient(Client& client);

		void addUser(Client& client);
		void addOperator(Client& client);
		//
		//getters
		size_t getUsernum() const;//number of users in the channel
		std::string getTopic() const;
		std::string getKey() const;
		std::string getChannelName() const;
		std::string getModes() const;
		void addMode(char mode);
		void removeMode(char mode);
		bool isrestrictTopic() const;
		bool isInviteOnly() const;
		bool hasMaxUsers() const;
		bool isPwdProtected() const;
		size_t maxNumOfUsers() const;
		Client* getClient(const std::string& nickName);
		const std::vector<Client>& getClientsVector() const;
		const std::vector<Client>& getOperatorsVector() const;
		void removeUser(Client& client);
		bool hasOperators(void);
		Client& oldestClientInChannel(void);




};

#endif