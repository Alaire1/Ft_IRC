#include "Channel.hpp"

Channel::Channel(std::string name) :_name(name), _topic(""), _key(""), _isInviteOnly(false), _hasMaxUsers(false), _restrictTopic(false), _pwdProtected(false){}

Channel::~Channel() {}

bool Channel::clientNotInChannel(Client& client)
{
    return std::find(_clients.begin(), _clients.end(), client) == _clients.end();
}

bool Channel::clientNotOperator(Client& client)
{
    return std::find(_operators.begin(), _operators.end(), client) == _operators.end();
}

void Channel::join(Client& client)
{
	if (clientNotInChannel(client))
    		_clients.push_back(client);
	else
		client.sendMessage("442 " + _name + " :You're already in that channel"); // 442 is a numeric code for sending a message that user is already in the channel
}

std::string Channel::getTopic() const{return _topic;}

std::vector<Client> Channel::getClientsVector() const{return _clients;}

std::vector<Client> Channel::getOperatorsVector() const{return _operators;}

std::string Channel::getChannelName() const{return _name;}

std::string Channel::getKey() const{return _key;}

bool Channel::isInviteOnly() const{return _isInviteOnly;}

bool Channel::hasMaxUsers() const{return _hasMaxUsers;}

bool Channel::isrestrictTopic() const{return _restrictTopic;}

bool Channel::isPwdProtected() const{return _pwdProtected;}

void Channel::invite(Client& client) // before that function in parsing we have to check if someone that is using the invite command is an operator //
{
    _invitees.push_back(client);
}

bool Channel::isInvitedToChannel(Client& client)
{
    return std::find(_invitees.begin(), _invitees.end(), client) != _invitees.end();
}

void Channel::addUser(Client& client)
{
    _clients.push_back(client);
}
void Channel::addOperator(Client& client)
{
    _operators.push_back(client);
}

void Channel::topicCommand(Client& client, std::string topic)
{
   if (topic == "")
   {
       //client.sendMessage("332 " + _name + " :" + _topic); // 332 is a numeric code for sending a topic
   }
   else
   {
       if (_restrictTopic && client.getNick() != _operatorNick)
       {
          // client.sendMessage("482 " + _name + " :You're not channel operator"); // 482 is a numeric code for sending a message that user is not a channel operator
       }
       else
       {
           _topic = topic;
         //  client.sendMessage("TOPIC " + _name + " :" + _topic); 
       }
   }
}

void Channel::kick(Client& ejectee)
{
	for (std::vector<Client>::iterator it = _clients.begin(); it != _clients.end(); ++it)
	{
		printf ("Clients include: %s\n", it->getNick().c_str());
		if (it->getNick() == ejectee.getNick())
		{
			_clients.erase(it);
			break;
		}
	}

	for (std::vector<Client>::iterator it = _operators.begin(); it != _operators.end(); ++it)
	{
		if (it->getNick() == ejectee.getNick())
		{
			_operators.erase(it);
			break;
		}
	}
}

void Channel::leave(Client& client)
{
    std::vector<Client>::iterator it = _clients.begin();
    for (; it != _clients.end(); ++it)
    {
        if (it->getNick() == client.getNick())
        {
            if (it->getNick() == _operatorNick && _clients.size() > 1)
            {
                _clients.erase(_clients.begin());
                setOperator(_clients[0]);
                // everything else that comes with setting a user as an operator
            }
            else
            {
                _clients.erase(it);
                // i have to send a message to all clients that someone left the channel
                //send("PART " + _name + " :" + client.getNick() + "!" + client.getUsername() + "@" + client.getHostname() + " has left " + _name); // nor sure if it's correct 
            }
            break;
        }
    }
}


bool Channel::clientWithThatNameNotInChannel(std::string name)
{
    for (std::vector<Client>::iterator it = _clients.begin(); it != _clients.end(); ++it)
    {
	   if (it->getNick() == name)
	   {
		  return false;
	   }
    }
    return true;
}

bool Channel::containsClient(Client& client)
{
    return std::find(_clients.begin(), _clients.end(), client) != _clients.end();
}
void Channel::setInviteOnly(bool isInviteOnly) //MODE +i or Mode -i
{
    _isInviteOnly = isInviteOnly;
}
void Channel::setRestrictTopic(bool restrictTopic) //MODE +t or Mode -t
{
    _restrictTopic = restrictTopic;
}
void Channel::setMaxUsers(int maxUsers) //MODE +l maxUsers 
{
    _maxUsers = maxUsers;
}
void Channel::setKey(std::string key) //MODE +k key
{
    this->_key = key;
}
void Channel::removeKey() //MODE -k
{
    _key = "";
}

void Channel::setOperator(Client& client) // before using it we have to check if someone that is setting is an operator himself
{
    _operatorNick = client.getNick();
}


void Channel::setTopic(std::string topic)
{
    _topic = topic;
}


size_t Channel::getUsernum() const
{
	return _clients.size();
}


size_t Channel::maxNumOfUsers() const {return _maxUsers;}


void Channel::removeInvite(Client& client)
{
    std::vector<Client>::iterator it = _invitees.begin();
    for (; it != _invitees.end(); ++it)
    {
	   if (it->getNick() == client.getNick())
	   {
		  _invitees.erase(it);
		  break;
	   }
    }
}

void Channel::removeOperator(Client& client)
{
    std::vector<Client>::iterator it = _operators.begin();
    for (; it != _operators.end(); ++it)
    {
	   if (it->getNick() == client.getNick())
	   {
		  _operators.erase(it);
		  break;
	   }
    }
}

void Channel::removeClient(Client& client)
{
    std::vector<Client>::iterator it = _operators.begin();
    for (; it != _operators.end(); ++it)
    {
	   if (it->getNick() == client.getNick())
	   {
		  _operators.erase(it);
		  break;
	   }
    }
}
