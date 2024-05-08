#include "Server.hpp" 


void Server::errorPrintGetaddrinfo(int status) {
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
void Server::errorSocketCreation(int status)
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

void Server::errorSetsockopt(int status)
{
    const std::map<int, std::string> setsockopt_errors = {
        {EACCES, "Permission denied: You lack the necessary permissions to set SO_REUSEADDR on the socket."},
        {EBUSY, "Resource busy: The SO_REUSEADDR option cannot be set because the socket is already bound or another operation is interfering."},
        {EBADF, "Bad file descriptor: The provided socket descriptor (socket_fd) is invalid."},
        {EINVAL, "Invalid argument: Either the value of 'reuse' is incorrect (should be 1) or the size of the data (sizeof(reuse)) is wrong."},
};
    std::cerr << "setsockopt error: ";
    if (setsockopt_errors.count(status) > 0) {
        std::cerr << setsockopt_errors.at(status);
    } else {
        std::cerr << "Unknown error";
    }
    std::cerr << std::endl;
}
void Server::errorFcntl(int status)
{
    const std::map<int, std::string> fcntl_errors = {
        {EBADF, "The file descriptor is not valid."},
        {EACCES, "The operation is prohibited by locks that other processes have on the file."},
        {EAGAIN, "The operation is prohibited by locks that other processes have on the file."},
        {EDEADLK, "The operation would cause a deadlock."},
        {EMFILE, "The process already has the maximum number of file descriptors open and tried to open more."},
        {ENOLCK, "There are too many segment locks open, lock tables are full, or a remote locking protocol failed to satisfy a request."},
        {EINVAL, "The third argument, cmd, is not valid, or the argument is F_SETLK or F_SETLKW and the data pointed to by lock is not valid."},
        {EINTR, "The function was interrupted by a signal."},
        {EIO, "An I/O error occurred."},
        {EFAULT, "lock points outside the process's allocated address space."},
        {ENOTTY, "The specified file does not support the requested command."}
    };
    std::cerr << "fcntl error: ";
    if (fcntl_errors.count(status) > 0) {
        std::cerr << fcntl_errors.at(status);
    } else {
        std::cerr << "Unknown error";
    }
    std::cerr << std::endl;
}

void Server::errorSocketBinding(int status)
{
    const std::map<int, std::string> bind_errors = {
        {EACCES, "The address is protected, and the user is not the superuser."},
        {EADDRINUSE, "The given address is already in use."},
        {EBADF, "The file descriptor is not a valid file descriptor."},
        {EINVAL, "The socket is already bound to an address, or the addrlen is wrong, or the socket was not in the AF_UNIX family."},
        {ENOTSOCK, "The file descriptor sockfd does not refer to a socket."},
        {EADDRNOTAVAIL, "The specified address is not available from the local machine."},
        {EFAULT, "The addr argument is not in a writable part of the user address space."}
    };
    std::cerr << "bind error: ";
    if (bind_errors.count(status) > 0) {
        std::cerr << bind_errors.at(status);
    } else {
        std::cerr << "Unknown error";
    }
    std::cerr << std::endl;
}

void Server::errorListen(int status)
{
    const std::map<int, std::string> listen_errors = {
        {EADDRINUSE, "Another socket is already listening on the same port."},
        {EBADF, "The socket is not a valid file descriptor."},
        {ENOTSOCK, "The file descriptor sockfd does not refer to a socket."},
        {EOPNOTSUPP, "The socket is not of a type that supports the listen() operation."},
        {EINVAL, "The socket is already connected."},
        {EACCES, "The calling process does not have the appropriate privileges to listen on the specified port."},
        {EINVAL, "The socket is not bound to an address."}
    };
    std::cerr << "listen error: ";
    if (listen_errors.count(status) > 0) {
        std::cerr << listen_errors.at(status);
    } else {
        std::cerr << "Unknown error";
    }
    std::cerr << std::endl;
}

