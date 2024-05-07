#ifndef SERVER_HPP
# define SERVER_HPP

#include <sys/socket.h>
#include <ostream>
#include <netdb.h>
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

        int    createSocket();
        int    getSocket() const;

        void   handleSignals();
        static void	signalHandler(int signum);
        
        
};
void printGetaddrinfoError(int status);
#endif
