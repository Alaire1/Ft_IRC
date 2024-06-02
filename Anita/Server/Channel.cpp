#include "Channel.hpp"
#include "Client.hpp"

Channel::Channel(std::string name) :_name(name), _topic("No topic set"), _isInviteOnly(false), _restrictTopic(false)
{
}

Channel::~Channel()
{
}

void Channel::join(Client& client)
{
    _clients.push_back(client);
}

void Channel::invite(Client& client) // before that function in parsing we have to check if someone that is using the invite command is an operator //
{
    _clients.push_back(client);
}
void Channel::topicCommand(Client& client, std::string topic = "")
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
void Channel::kick(Client& client) // before that function in parsing we have to check if someone that is using the kick command is an operator //
{
    client_kicked = false; 
    for (std::vector<Client>::iterator it = _clients.begin() ; it != _clients.end();)
    {
        if (it->getNick() == client.getNick()) 
        {
            it = _clients.erase(it);
            client_kicked = true;
        }
        else
        {
            ++it;
        }
    }
    if (client_kicked)
    {
        client.send("KICK " + _name + " " + client.getNick() + " :You have been kicked from " + _name);
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
    _key = key;
}
void Cahnnel::removeKey() //MODE -k
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

