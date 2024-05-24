#include <iostream>
#include <vector>
#include <string>
#include <poll.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <fcntl.h>
#include <unistd.h>
#include <cstring>
#include <arpa/inet.h>

class Server {
public:
    Server(int port);
    void startServer();
    void handleNewConnection();
    void handleExistingConnection(int fd);
    void closeFds();

private:
    int _socket;
    int _port;
    std::vector<struct pollfd> _fds;
    bool _signal;
};
