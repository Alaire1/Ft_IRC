#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
#include <cmath>

std::vector<std::string> valid_commands;

bool isValidChannelName(std::string channel_name)
{
	if (channel_name.size() == 0)
	{
		return false;
	}
	if (channel_name.size() > 50)
	{
		return false;
	}
	if ((channel_name[0] != '#') &&  (channel_name[0] != '&') &&  (channel_name[0] != '+') && (channel_name[0] != '!'))
	{
		return false;
	}
	if ((channel_name.find(' ') != std::string::npos) || ((channel_name.find(',') != std::string::npos)) || (channel_name.find(7) != std::string::npos))
	{
		return false;
	}
	return true;
}

void channelJoiningAttempt(std::string prefix, std::vector<std::string> params)
{
	if (params.size() == 0)
	{
		std::cout << "Not enough parameters" << std::endl;
		return ;
	}
	if (params.size() > 0)
	{
		if (isValidChannelName(params[0]) == false)
		{
			std::cout << "Invalid channel name" << std::endl;
			return ;
		}
		std::cout << "Channel name is valid" << std::endl;
	}
	if (channelExists(params[0]))
	{
		std::cout << "Channel exists. THE CLIENT WILL TRY TO JOIN IT" << std::endl;
	}
	else
	{
		std::cout << "Channel does not exist. THE CLIENT WILL TRY TO CREATE IT" << std::endl;
	
	}

}


void ircMessageParser (std::string message)
{
	std::string prefix;
	std::string command;
	std::vector<std::string> params;
	std::string trailing;

	if (message[0] == ':')
		prefix = message.substr(1, message.find(' ') - 1);

	command = message.substr(0, message.find(' '));
	message = message.substr(message.find(' ') + 1);

	while (message[0] == ':')
	{
		params.push_back(message.substr(1, message.find(' ') - 1));
		message = message.substr(message.find(' ') + 1);
	}

	while (message.find(' ') != std::string::npos)
	{
		 params.push_back(message.substr(0, message.find(' ')));
		 message = message.substr(message.find(' ') + 1);
	}

	trailing = message;

	std::cout << "Prefix: " << prefix << std::endl;
	std::cout << "Command: " << command << std::endl;
	for (int i = 0; i < params.size(); i++)
	{
		std::cout << "Param " << i << ": " << params[i] << std::endl;
	}
	std::cout << "Trailing: " << trailing << std::endl;

	if (std::find(valid_commands.begin(), valid_commands.end(), command) != valid_commands.end())
	{
		std::cout << "Command is valid" << std::endl;
	}
	else
	{
		std::cout << "Command is invalid" << std::endl;
		return ;
	}
	if (command == "JOIN")
	{
		channelJoiningAttempt(prefix, params);
	}
	

}

std::vector<std::string> listValidCommands()
{
 	valid_commands.push_back("JOIN");
 	valid_commands.push_back("PART");
 	valid_commands.push_back("PRIVMSG");
 	valid_commands.push_back("QUIT");
 	valid_commands.push_back("NICK");
 	valid_commands.push_back("USER");
 	valid_commands.push_back("KICK");
 	valid_commands.push_back("MODE");
 	valid_commands.push_back("INVITE");
 	valid_commands.push_back("TOPIC");
 	valid_commands.push_back("PASS");
 	valid_commands.push_back("NOTICE");
 	valid_commands.push_back("WHO");
	return (valid_commands);
}
int main (int argc, char **argv, char **env)
{
		std::vector <std::string> validCommands = listValidCommands();
		std::string message = ":irc.example.net JOIN #animals";
		ircMessageParser(message);
		return 0;

}