#include "Server.hpp" 


void Server::printGetaddrinfoError(int status) {
    std::map<int, std::string> errorMessages = {
        {EAI_AGAIN, "Temporary failure in name resolution"},
        {EAI_BADFLAGS, "Invalid value for ai_flags"},
        {EAI_FAIL, "Non-recoverable failure in name resolution"},
        {EAI_FAMILY, "The ai_family does not support the specified ai_socktype"},
        {EAI_MEMORY, "Memory allocation failure"},
        {EAI_NONAME, "Neither nodename nor servname provided, or not known"},
        {EAI_SERVICE, "The ai_socktype is not supported for the specified ai_family or the specified service is not supported for the specified ai_socktype"},
        {EAI_SOCKTYPE, "The specified ai_socktype is not supported"},
        {EAI_OVERFLOW, "Argument buffer overflow"}
    };
    std::cerr << "getaddrinfo error: ";
    if (errorMessages.count(status) > 0) {
        std::cerr << errorMessages[status];
    } else {
        std::cerr << "Unknown error";
    }
    std::cerr << std::endl;
    exit(1);
}
void Server::socketCReationError(int status)
{
    std::map<int, std::string> errorMessages = {
        {EAI_AGAIN, "Temporary failure in name resolution"},
        {EAI_BADFLAGS, "Invalid value for ai_flags"},
        {EAI_FAIL, "Non-recoverable failure in name resolution"},
        {EAI_FAMILY, "The ai_family does not support the specified ai_socktype"},
        {EAI_MEMORY, "Memory allocation failure"},
        {EAI_NONAME, "Neither nodename nor servname provided, or not known"},
        {EAI_SERVICE, "The ai_socktype is not supported for the specified ai_family or the specified service is not supported for the specified ai_socktype"},
        {EAI_SOCKTYPE, "The specified ai_socktype is not supported"},
        {EAI_OVERFLOW, "Argument buffer overflow"}
    };
    std::cerr << "Socket creation error: ";
    if (errorMessages.count(status) > 0) {
        std::cerr << errorMessages[status];
    } else {
        std::cerr << "Unknown error";
    }
    std::cerr << std::endl;
    exit(1);
}