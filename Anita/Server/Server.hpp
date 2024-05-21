#ifndef SERVER_HPP
# define SERVER_HPP
#include <sys/socket.h>
#include <ostream>
#include <netdb.h>
#include <signal.h>
#include <iostream>
#include <cstring>
#include <string.h>
#include <map>
#include <arpa/inet.h> //-> for inet_ntoa()
#include <errno.h>
#include <poll.h>
#include <fcntl.h>
#include <vector>
#include <cstdlib>
#include "../Client/Client.hpp"

#define BACKLOG 10 // common choice between 5 and 10 // may be changed
class Server
{
    private:
        struct addrinfo *_servInfo; // struct needed to create a socket
        int _socket;
				static bool _signal; //-> static boolean for signal
        bool _isRunning;
        std::string _password;
        int _port;
        std::vector<struct Client> _clients;
        std::vector<struct pollfd> _fds;
    
    public:
        Server();
        Server(int port, std::string password);
        ~Server();
     
        void    createServer();

        // 	starting server & accepting clients/data
        void   	startServer();
        int 		pollFds();
				void 		handleExistingConnection(int fd);
				void 		handleNewConnection();
				void 		addFd(int fd, short events);
				void 		setIpAdd(std::string ipadd);
        
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
				void 		closeFds();
				void 		clearClients(int fd);

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
        void    errorPoll(int status);
//				void 		errorAccept(int status);

        //testing
        void    printPassword();
       
};

#endif
