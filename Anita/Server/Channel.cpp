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

void Channel::kick(Client& client) // before that function in parsing we have to check if someone that is using the kick command is an operator //
{ 
    for (std::vector<Client>::iterator it = _clients.begin() ; it != _clients.end();)
    {
        if (it->getNick() == client.getNick()) 
        {
            it = _clients.erase(it);
        }
        else
        {
            ++it;
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
                _operatorNick = _clients[0].getNick();
                // everything else that comes with setting a user as an operator
            }
            else
            {
                _clients.erase(it);
            }
            break;
        }
    }
}