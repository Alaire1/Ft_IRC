#ifndef CHANNEL_HPP
# define CHANNEL_HPP
#include <vector>
#include <string>
#include <iostream>
#include "Client.hpp"
#include "Server.hpp"

class Channel
{
    private:
        std::string _name;
        std::string _topic; 
        std::string _key; // password

        bool _isInviteOnly;
        bool _restrictTopic;
        int _maxUsers;
        
        std::vector<Client> _clients;
        std::string _operatorNick;

    public:
        Channel(std::string name);
        ~Channel();
        void join(Client& client); 
        void kick(Client& client); 
        void leave(Client& client);
        void invite(Client& client);

        //setters
        void setInviteOnly(bool isInviteOnly);
        void setRestrictTopic(bool restrictTopic);
        void setMaxUsers(int maxUsers);

        void setOperator(Client& client);
        void setTopic(std::string topic);
	   void setKey(std::string key);

};

#endif