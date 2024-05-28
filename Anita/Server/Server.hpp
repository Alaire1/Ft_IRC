#ifndef SERVER_HPP
# define SERVER_HPP
#include <sys/socket.h>
#include <ostream>
#include <cstdlib>
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
#include "Client.hpp"

#define BACKLOG 10 // common choice between 5 and 10 // may be changed
class Server
{
    private:
				static bool 			_signal; //-> static boolean for signal
        int 							_servSocket;
        int 							_port;
        std::string 			_password;
        std::vector<Client> _clients;
        std::vector<pollfd> _fds;

    public:
        Server();
        Server(int port, std::string password);
        ~Server();


        // 	starting server & accepting clients/data
        void    createServer();// server initialization
        void    createAndSetSocket();
        void   	startServer();
				void 		handleNewConnection();
				void 		handleExistingConnection(int fd);

				//Functions
				void 		addFd(int fd, short events);
        int 		pollFds();
				
        // socket handling

        //void    createSocket();
        int     getSocket() const;
        //signal handling

        static void	signalHandler(int signum);

        //terminating
				void 		closeFds();
				void 		clearClients(int fd);
      

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
