#ifndef SERVER_HPP
# define SERVER_HPP

#include <sys/socket.h>
#include <ostream>
#include <netdb.h>
#include <signal.h>
#include <iostream>
#include <cstring>
#include <map>
#include <errno.h>
#include <fcntl.h>

class Server
{
    private:
        struct addrinfo *_servInfo; // struct needed to create a socket
        int _socket;
        bool _isRunning;
    public:
        Server();
        ~Server();
     
        void    createServer();
        // socket handling
        void    initializeHints();
        int    createSocket();
        int    getSocket() const;

        //signal handling
        void   handleSignals();
        static void	signalHandler(int signum);
        //error handling
        void    socketCreationError(int status);
        void    printGetaddrinfoError(int status);
       
};
#endif
