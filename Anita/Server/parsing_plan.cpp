void parser(std::string clentData, Client& client)
{
    // *spliting the client data by \r\n
   //std::vector <std::string> = splitDataByDelimiter(clentData); 
   // *  removing \r\n from the end of the string
    // * split invidual strings into componens 1. prefix, 2. command, 3. params
    // * only 3 commands can be used without registration : // we will need a variable to indicate if the client is registered or not
            // - NICK
            // - USER
            // - PASS
        // rest should return an error/ information that you have to be registered first ,which is giving all the 3 info that is correct
    // * if the command is NICK, check if the nickname is valid // function prototyoe below
    // * if the command is USER, check if the username is valid
    // nc(no slashes) : // Hex chat sending by itself
    //  NICK yournickname
     // USER yourusername 0 * :Your Real Name
     // PASS yourpassword
    // * if the command is USER, check if the username is valid
    // * if the command is PASS, matches the servers password 
            // if not return ERROR : Invalid password
                  // that should not end the loop as in nc you can try with password again, maybe there'a a way to do it in hex chat too // have asked copilot and there's no way to send the password again
    // is client is registered then we can use other commands
         // :prefix command params // prefix
         // JOIN #channel
          //  PART #channel
          //  PRIVMSG #channel :message or PRIVMSG nickname :message //
          // TOPIC #channel :new topic or TOPIC #channel
          // QUIT :message
          // PART #channel
          // KICK #channel nickname :message
          // INVITE nickname #channel 
          //MODE 
              // MODE #channel +o nickname, etc.
  
    // sending messages whe have to add prefix to the message beacuse clinet is sending stuff without prefix to the server, then server is fowarding the message to channle or a clinet and then we have to add prefix to the message
  
    

    
}
 // Limitation for the channel name ?? // 
 

// optionally handling WHO cammand that is used to get the list of users in the channel or server
// WHO #channel OR WHO (for server)
#include <iostream>
#include <cctype>
#include <string>
// what restrictions are for the Username? // horse doesn't say anything 
bool isValidIrcNickname(const std::string& nickname) {
  if (nickname.empty()) {

    return false;
  }
  if (nickname[0] == '$' || nickname[0] == ':') {
    return false; 
  }

  for (char c : nickname) {
    if (std::isspace(c) || c == ',' || c == '*' || c == '?' || c == '!' || c == '@') {
      return false; 
    }
  }

  // Dot character check? 
  /*
  for (char c : nickname) {
    if (c == '.') {
      return false; // Nickname should not contain dots (optional)
    }
  }
  */

  return true;
}

int main() {
  std::string nickname;
  std::cout << "Enter a nickname: ";
  std::getline(std::cin, nickname);

  if (isValidIrcNickname(nickname)) {
    std::cout << nickname << " is a valid IRC nickname." << std::endl;
  } else {
    std::cout << nickname << " is not a valid IRC nickname." << std::endl;
  }

  return 0;
}

// Limitation for the channel name 
bool isValidChannelName(const std::string& channelName) {
    if (channelName.empty()) {
        return false;
    }
    //checksize() additional restriction for the channel name length
    char firstChar = channelName[0];
    if (firstChar != '#' && firstChar != '&') {
        return false;
    }
    for (size_t i = 1; i < channelName.size(); ++i) {
        char c = channelName[i];
        if (c == ' ' || c == '\x07' || c == ',') {
            return false;
        }
    }
    return true;
}