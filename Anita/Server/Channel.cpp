#include "Channel.hpp"

Channel::Channel(std::string name) :_name(name), _topic(""), _key(""), _modes("+"), _isInviteOnly(false), _hasMaxUsers(false), _restrictTopic(false), _pwdProtected(false){}

Channel::~Channel() {}

bool Channel::clientNotInChannel(Client& client)
{
    return std::find(_clients.begin(), _clients.end(), client) == _clients.end();
}

bool Channel::clientNotOperator(Client& client)
{
    return std::find(_operators.begin(), _operators.end(), client) == _operators.end();
}



std::string Channel::getTopic() const{return _topic;}

const std::vector<Client>& Channel::getClientsVector() const{return _clients;}

const std::vector<Client>& Channel::getOperatorsVector() const{return _operators;}

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
void Channel::addMode(char mode) {
    
    if (_modes.find(mode) == std::string::npos) {
        _modes += mode; 
    }
}
void Channel::setIfHasPassword(bool mode)
{
    this->_pwdProtected = mode;
}

void Channel::removeMode(char mode) {
    if (_modes.find(mode) != std::string::npos) {
        _modes.erase(_modes.find(mode), 1);
    }
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

bool Channel::hasOperators(void)
{
    if (_operators.size() == 0)
        return false;
    return true;
}


Client& Channel::oldestClientInChannel(void)
{
    return (_clients[0]);
}

void Channel::removeUser(Client& client)
{
    std::vector<Client>::iterator it = _clients.begin();
    for (; it != _clients.end(); ++it)
    {
	   if (*it == client)
	   {
		  _clients.erase(it);
		  break;
	   }
    }
    std::vector<Client>::iterator ita = _operators.begin();
    for (; ita != _operators.end(); ++it)
    {
	   if (*ita == client)
	   {
		  _operators.erase(ita);
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
		  return false;
    }
    return true;
}

int Channel::checkIfOperatorleft()//Checks if there is still operators left, if not assigns the _clients[0] as operator
{
	if(!_operators.size())
	{
		if(_clients.size())
		{
			_operators.push_back(_clients[0]);
			return 1;
		}
	}
	return 0;
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

std::string Channel::getModes() const
{
    return _modes;
}

size_t Channel::maxNumOfUsers() const {return _maxUsers;}

void Channel::sethasMaxUsers(bool setter){_hasMaxUsers = setter;}

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

int Channel::removeOperator(Client& client)
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
		if(checkIfOperatorleft())
			return 1;
		else
			return 0;
}

void Channel::removeClient(Client& client)
{
	std::vector<Client>::iterator it = _clients.begin();
	for (; it != _clients.end(); ++it)
	{
		if (it->getNick() == client.getNick())
		{
			_clients.erase(it);
			break;
		}
	}

	std::vector<Client>::iterator iti = _invitees.begin();
	for (; iti != _invitees.end(); ++iti)
	{
		if (iti->getNick() == client.getNick())
		{
			_invitees.erase(it);
			break;
		}
	}
	//long unsigned int i = -1;
	//while (++i < _invitees.size())
	//{
	//	if (_invitees[i].getNick() == client.getNick())
	//	{
	//		_invitees.erase(_invitees.begin() + i);
	//		break;
	//	}
	//}
	//if(!_clients.size())
	//	clearVectors();
}

void Channel::clearVectors()
{
	_clients.clear();
	_operators.clear();
	_invitees.clear();
}


 Client* Channel::getClient(const std::string& nickName) 
{
    std::vector<Client>::iterator it = _clients.begin();
    for (; it != _clients.end(); ++it)
		{
			if (it->getNick() == nickName)
				return &(*it);
		}
	return NULL;
}

void Channel::updateNick(Client& client, std::string& newNick)
{
    std::vector<Client>::iterator it;
    for (it = _clients.begin(); it != _clients.end(); ++it)
    {
	   if (it->getNick() == client.getNick())
			 (*it).setNickName(newNick);
    }
    for (it = _invitees.begin(); it != _invitees.end(); ++it)
    {
	   if (it->getNick() == client.getNick())
			 (*it).setNickName(newNick);
    }
    for (it = _operators.begin(); it != _operators.end(); ++it)
    {
	   if (it->getNick() == client.getNick())
			 (*it).setNickName(newNick);
    }
}
	//update names in all lists: Channel: operators, invitees 
	
