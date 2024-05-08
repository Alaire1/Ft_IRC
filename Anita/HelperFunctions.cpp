#include "HelperFunctions.hpp"

void checkIfPortIsValid(char *port)
{
    int portNumber = atoi(port);
    if (portNumber < 1024 || portNumber > 65535)
    {
        std::cerr << "Invalid port number. Give number between 1024 and 65535" << std::endl;
         exit (1);
    }
   
}
// if we want we can put some rules of password here 