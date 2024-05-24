#include "Server.hpp"

Server::Server(int port) : _port(port), _signal(false) {
    _socket = socket(AF_INET, SOCK_STREAM, 0);
    if (_socket < 0) {
        perror("Socket creation failed");
        exit(EXIT_FAILURE);
    }

    int opt = 1;
    if (setsockopt(_socket, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0) {
        perror("Set socket options failed");
        close(_socket);
        exit(EXIT_FAILURE);
    }

    struct sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = INADDR_ANY;
    addr.sin_port = htons(_port);

    if (bind(_socket, (struct sockaddr *)&addr, sizeof(addr)) < 0) {
        perror("Bind failed");
        close(_socket);
        exit(EXIT_FAILURE);
    }

    if (listen(_socket, SOMAXCONN) < 0) {
        perror("Listen failed");
        close(_socket);
        exit(EXIT_FAILURE);
    }

    struct pollfd serverPollFd;
    serverPollFd.fd = _socket;
    serverPollFd.events = POLLIN;
    _fds.push_back(serverPollFd);
}

void Server::startServer() {
    std::cout << "Server < " << _socket << " > waiting for connection... " << std::endl;

    while (!_signal) {
        int ready = poll(_fds.data(), _fds.size(), -1);
        if (ready == -1) {
            perror("Poll failed");
            break;
        }

        for (auto it = _fds.begin(); it != _fds.end(); ) {
            if (it->revents & POLLIN) {
                if (it->fd == _socket) {
                    handleNewConnection();
                } else {
                    handleExistingConnection(it->fd);
                }
            }
            if (it->fd == -1) {
                it = _fds.erase(it);
            } else {
                ++it;
            }
        }
    }
    closeFds();
}

void Server::handleNewConnection() {
    struct sockaddr_in addr;
    socklen_t addrLen = sizeof(addr);
    int clientSocket = accept(_socket, (struct sockaddr *)&addr, &addrLen);
    if (clientSocket < 0) {
        perror("Accept failed");
        return;
    }

    if (fcntl(clientSocket, F_SETFL, O_NONBLOCK) < 0) {
        perror("Set non-blocking failed");
        //close(clientSocket);
        return;
    }

    struct pollfd clientPollFd;
    clientPollFd.fd = clientSocket;
    clientPollFd.events = POLLIN;
    clientPollFd.revents = 0;
    _fds.push_back(clientPollFd);

    std::cout << "Client connected: " << clientSocket << std::endl;
}

void Server::handleExistingConnection(int fd) {
    char buffer[1024];
    memset(buffer, 0, sizeof(buffer));
    int bytes = recv(fd, buffer, sizeof(buffer), 0);
    if (bytes < 0) {
        perror("Recv failed");
        close(fd);
        fd = -1; // Mark for removal
    } else if (bytes == 0) {
        std::cout << "Client disconnected: " << fd << std::endl;
        close(fd);
        fd = -1; // Mark for removal
    } else {
        std::cout << "Received data: " << std::string(buffer, bytes) << std::endl;
        // Handle incoming data
    }
}

void Server::closeFds() {
    for (auto &pfd : _fds) {
        if (pfd.fd != -1) {
            close(pfd.fd);
        }
    }
}

int main() {
    Server server(6667); // Change port as needed
    server.startServer();
    return 0;
}
