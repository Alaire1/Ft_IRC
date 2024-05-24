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
				static bool 			_signal; //-> static boolean for signal
        int 							_socket;
        int 							_port;
        std::string 			_password;
				struct addrinfo*  _servInfo;
        std::vector<struct pollfd> _fds;
        std::vector<struct Client> _clients;

    public:
        Server();
        Server(int port, std::string password);
        ~Server();


        // 	starting server & accepting clients/data
        void    createServer();
        void   	startServer();
				void 		handleNewConnection();
				void 		handleExistingConnection(int fd);
				//void 		clientAccept();

				//Functions
				void 		addFd(int fd, short events);
        int 		pollFds();
				
        // socket handling
        void    initializeHints();
        void    createAndSetSocket();
        //void    createSocket();
        int     getSocket() const;
        //void    setSocketReusable();
        //void    nonBlockingSocket();
        //void    bindSocket();
        //void    listenSocket();
        //void    initialize_pollfd();
				void 		closeFds();
				void 		clearClients(int fd);

        //signal handling
        void   handleSignals();
        static void	signalHandler(int signum);
        static void   handle_sigstop(int sig);
      

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
				void 		printclientfds(std::vector<struct Client> clients);
				void    printfds(std::vector<struct pollfd> fds);

};

#endif
