#include "Server.hpp"
#include "Server.cpp"
#include <signal.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <iostream>
#include <sys/socket.h>
#include <ostream>

int main()
{
    Server server;

    server.createServer();
    std::cout << "socket: " << server.getSocket() << std::endl;
    return (0);
}
