#include "HelperFunctions.hpp"
#include <cstdlib>
#include <cstring>

bool isNumeric(const std::string& str) {
  for (size_t i = 0; i < str.length(); ++i) {
    if (str[i] < '0' || str[i] > '9') {
      return false;
    }
  }
  return true;
}

void checkIfPortIsValid(char *port)
{ 
    if (isNumeric(port) == false)
    {
        std::cerr << "Invalid port number. Must be fully numeric" << std::endl;
        exit (1);
    }
    int portNumber = atoi(port);
    if (portNumber <= 1024 || portNumber >= 65535)
    {
        std::cerr << "Invalid port number. Give number between 1024 and 65535" << std::endl;
         exit (1);
    }
}

bool isNotPrintable(char *password)
{
    for (size_t i = 0; i < strlen(password); ++i) {
        if (password[i] < 32 || password[i] > 126) {
            return true;
        }
    }
    return false;
}

void checkPassword(char *password)
{
    if (strlen(password) < 1)
    {
        std::cerr << "Password must be at least 1 character long" << std::endl;
        exit (1);
    }
    if (isNotPrintable(password) == true)
    {
        std::cerr << "Password must contain printable characters only" << std::endl;
        exit (1);
    }
  
}
// if we want we can put some rules of password here c++

