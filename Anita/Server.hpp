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
#include <cstdlib>

#define BACKLOG 10 // common choice between 5 and 10 // may be changed
class Server
{
    private:
        struct addrinfo *_servInfo; // struct needed to create a socket
        int _socket;
        bool _isRunning;
        std::string _password;
        int _port;
        std::vector<struct pollfd> _fds;
    
    public:
        Server();
        Server(int port, std::string password);
        ~Server();
     
        void    createServer();
        void   startServer();
        // socket handling
        void    initializeHints();
        void    createSocket();
        int     getSocket() const;
        void    setSocketReusable();
        void    nonBlockingSocket();
        void    bindSocket();
        void    listenSocket();
        int     createAndSetSocket();
        void    initialize_pollfd();

        //signal handling
        void   handleSignals();
        static void	signalHandler(int signum);

        //error handling
        void    errorPrintGetaddrinfo(int status);
        void    errorSocketCreation(int status);
        void    errorSetsockopt(int status);
        void    errorFcntl(int status);
        void    errorSocketBinding(int status);
        void    errorListen(int status);
        void   errorPoll(int status);

        //testing
        void    printPassword();
       
};
#endif
