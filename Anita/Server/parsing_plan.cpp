void parser(std::string clentData, Client& client)
{
    // *spliting the clinet data by \r\n
   //std::vector <std::string> = splitDataByDelimiter(clentData); 
   // *  removing \r\n from the end of the string
    // * split invidual strings into componens 1. prefix, 2. command, 3. params
    // * only 3 commands are working for now
           // - NICK
            // - USER
            // - PASS
        // rest should return an error/ information that you have to be registered first ,which is giving all thye 3 info that is correct
    // * if the command is NICK, check if the nickname is valid // function prototyoe below
    // * if the command is USER, check if the username is valid
    // * if the command is PASS, matches the servers password 
            // if not return ERROR : Invalid password
                  // that should not end the loop as in nc you can try with password again, maybe there'a a way to do it in hex chat too
    
    

    
}



#include <iostream>
#include <cctype>
#include <string>
// what restrictions are for the Username? //horse doesn't say anymthing 
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