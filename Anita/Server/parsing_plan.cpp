// void parser(std::string clentData, Client& client)
// {
//     // *spliting the client data by \r\n
//    //std::vector <std::string> = splitDataByDelimiter(clentData); 
//    // *  removing \r\n from the end of the string
//     // * split invidual strings into componens 1. prefix, 2. command, 3. params
//     // * only 3 commands can be used without registration : // we will need a variable to indicate if the client is registered or not
//             // - NICK
//             // - USER
//             // - PASS
//         // rest should return an error/ information that you have to be registered first ,which is giving all the 3 info that is correct
//     // * if the command is NICK, check if the nickname is valid // function prototyoe below
//     // * if the command is USER, check if the username is valid
//     // nc(no slashes) : // Hex chat sending by itself
//     //  NICK yournickname
//      // USER yourusername 0 * :Your Real Name
//      // PASS yourpassword
//     // * if the command is USER, check if the username is valid
//     // * if the command is PASS, matches the servers password 
//             // if not return ERROR : Invalid password
//                   // that should not end the loop as in nc you can try with password again, maybe there'a a way to do it in hex chat too // have asked copilot and there's no way to send the password again
//     // is client is registered then we can use other commands
//          // :prefix command params // prefix
//          // JOIN #channel
//           //  PART #channel
//           //  PRIVMSG #channel :message or PRIVMSG nickname :message //
//           // TOPIC #channel :new topic or TOPIC #channel
//           // QUIT :message
//           // PART #channel
//           // KICK #channel nickname :message
//           // INVITE nickname #channel 
//           //MODE 
//               // MODE #channel +o nickname, etc.
  
//     // sending messages whe have to add prefix to the message because client is sending stuff without prefix to the server, then server is fowarding the message to channle or a clinet and then we have to add prefix to the message
//   // build a reply : + sender + " " + command + " " + params + "\r\n"
    

    
// }
 // Limitation for the channel name ?? // 
 

// optionally handling WHO cammand that is used to get the list of users in the channel or server
// WHO #channel OR WHO (for server)
#include <iostream>
#include <cctype>
#include <string>
#include "Channel.hpp"
#include "Server.hpp"
#include "Client.hpp"

// what restrictions are for the Username? // horse doesn't say anything 


//COMMENTED OUT BY NIKOLA TO TRY TO GET CHANNEL JOINING TO WORK


// bool isValidIrcNickname(const std::string& nickname) {
//   if (nickname.empty()) {

//     return false;
//   }
//   if (nickname[0] == '$' || nickname[0] == ':') {
//     return false; 
//   }

//   for (char c : nickname) {
//     if (std::isspace(c) || c == ',' || c == '*' || c == '?' || c == '!' || c == '@') {
//       return false; 
//     }
//   }

//   // Dot character check? 
//   /*
//   for (char c : nickname) {
//     if (c == '.') {
//       return false; // Nickname should not contain dots (optional)
//     }
//   }
//   */

//   return true;
// }

// int main() {
//   std::string nickname;
//   std::cout << "Enter a nickname: ";
//   std::getline(std::cin, nickname);

//   if (isValidIrcNickname(nickname)) {
//     std::cout << nickname << " is a valid IRC nickname." << std::endl;
//   } else {
//     std::cout << nickname << " is not a valid IRC nickname." << std::endl;
//   }

//   return 0;
// }

// // Limitation for the channel name 
// bool isValidChannelName(const std::string& channelName) {
//     if (channelName.empty()) {
//         return false;
//     }
//     //checksize() additional restriction for the channel name length
//     char firstChar = channelName[0];
//     if (firstChar != '#' && firstChar != '&') {
//         return false;
//     }
//     for (size_t i = 1; i < channelName.size(); ++i) {
//         char c = channelName[i];
//         if (c == ' ' || c == '\x07' || c == ',') {
//             return false;
//         }
//     }
//     return true;
// }

std::vector<std::string> valid_commands;

std::vector<std::string> listValidCommands() //should be moved to server's constructor
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

// void channelJoiningAttempt(std::string &prefix, std::vector<std::string> &params, Server& server)
// {
// 	std::string placeholder_for_using_prefix_so_that_it_does_not_give_warning = prefix;
// 	if (params.size() <= 0)
// 	{
// 		std::cout << "Not enough parameters" << std::endl;
// 		return ;
// 	}
	
// 	if (isValidChannelName(params[0]) == false)
// 	{
// 		std::cout << "Invalid channel name" << std::endl;
// 		return ;
// 	}
// 	std::cout << "Channel name is valid" << std::endl;	
// 	if (server.channelExists(params[0]))
// 	{
// 		std::cout << "Channel exists. THE CLIENT WILL TRY TO JOIN IT" << std::endl;
// 		server.joinChannel(prefix, params[0]);
// 	}
// 	else
// 	{
// 		std::cout << "Channel does not exist. THE CLIENT WILL TRY TO CREATE IT" << std::endl;
	
// 	}

// }


// void ircMessageParser (std::string message, Server& server)
// {
// 	std::string prefix;
// 	std::string command;
// 	std::vector<std::string> params;
// 	std::string trailing;

// 	listValidCommands(); //should be moved to server's constructor

// 	if (message[0] == ':')
// 		prefix = message.substr(1, message.find(' ') - 1);

// 	command = message.substr(0, message.find(' '));
// 	message = message.substr(message.find(' ') + 1);

// 	while (message[0] == ':')
// 	{
// 		params.push_back(message.substr(1, message.find(' ') - 1));
// 		message = message.substr(message.find(' ') + 1);
// 	}

// 	while (message.find(' ') != std::string::npos)
// 	{
// 		 params.push_back(message.substr(0, message.find(' ')));
// 		 message = message.substr(message.find(' ') + 1);
// 	}

// 	trailing = message;

// 	std::cout << "Prefix: " << prefix << std::endl;
// 	std::cout << "Command(" << command << ")" << std::endl;
// 	for (unsigned long i = 0; i < params.size(); i++)
// 	{
// 		std::cout << "Param " << i << ": " << params[i] << std::endl;
// 	}
// 	std::cout << "Trailing: " << trailing << std::endl;

// 	if (std::find(valid_commands.begin(), valid_commands.end(), command) != valid_commands.end())
// 	{
// 		std::cout << "Command is valid" << std::endl;
// 	}
// 	else
// 	{
// 		std::cout << "Command is invalid" << std::endl;
// 		return ;
// 	}
// 	if (command == "JOIN")
// 	{
// 		channelJoiningAttempt(prefix, params, server);
// 	}
	

// }

