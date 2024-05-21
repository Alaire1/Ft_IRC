#include "Server.hpp"

void Server::errorPrintGetaddrinfo(int status) {
    std::map<int, std::string> errorMessages;
    errorMessages[EAI_AGAIN] = "Temporary failure in name resolution";
    errorMessages[EAI_BADFLAGS] = "Invalid value for ai_flags";
    errorMessages[EAI_FAIL] = "Non-recoverable failure in name resolution";
    errorMessages[EAI_FAMILY] = "The ai_family does not support the specified ai_socktype";
    errorMessages[EAI_MEMORY] = "Memory allocation failure";
    errorMessages[EAI_NONAME] = "Neither nodename nor servname provided, or not known";
    errorMessages[EAI_SERVICE] = "The ai_socktype is not supported for the specified ai_family or the specified service is not supported for the specified ai_socktype";
    errorMessages[EAI_SOCKTYPE] = "The specified ai_socktype is not supported";
    errorMessages[EAI_OVERFLOW] = "Argument buffer overflow";

    std::cerr << "getaddrinfo error: ";
    std::map<int, std::string>::const_iterator it = errorMessages.find(status);
    if (it != errorMessages.end()) {
        std::cerr << it->second;
    } else {
        std::cerr << "Unknown error";
    }
    std::cerr << std::endl;
}
void Server::errorSocketCreation(int status)
{
    std::map<int, std::string> errorMessages;
    errorMessages[EAI_AGAIN] = "Temporary failure in name resolution";
    errorMessages[EAI_BADFLAGS] = "Invalid value for ai_flags";
    errorMessages[EAI_FAIL] = "Non-recoverable failure in name resolution";
    errorMessages[EAI_FAMILY] = "The ai_family does not support the specified ai_socktype";
    errorMessages[EAI_MEMORY] = "Memory allocation failure";
    errorMessages[EAI_NONAME] = "Neither nodename nor servname provided, or not known";
    errorMessages[EAI_SERVICE] = "The ai_socktype is not supported for the specified ai_family or the specified service is not supported for the specified ai_socktype";
    errorMessages[EAI_SOCKTYPE] = "The specified ai_socktype is not supported";
    errorMessages[EAI_OVERFLOW] = "Argument buffer overflow";

    std::cerr << "Socket creation error: ";
    std::map<int, std::string>::const_iterator it = errorMessages.find(status);
    if (it != errorMessages.end()) {
        std::cerr << it->second;
    } else {
        std::cerr << "Unknown error";
    }
    std::cerr << std::endl;
}

void Server::errorSetsockopt(int status)
{
    std::map<int, std::string> setsockopt_errors;
    setsockopt_errors[EACCES] = "Permission denied: You lack the necessary permissions to set SO_REUSEADDR on the socket.";
    setsockopt_errors[EBUSY] = "Resource busy: The SO_REUSEADDR option cannot be set because the socket is already bound or another operation is interfering.";
    setsockopt_errors[EBADF] = "Bad file descriptor: The provided socket descriptor (socket_fd) is invalid.";
    setsockopt_errors[EINVAL] = "Invalid argument: Either the value of 'reuse' is incorrect (should be 1) or the size of the data (sizeof(reuse)) is wrong.";

    std::cerr << "setsockopt error: ";
    std::map<int, std::string>::const_iterator it = setsockopt_errors.find(status);
    if (it != setsockopt_errors.end()) {
        std::cerr << it->second;
    } else {
        std::cerr << "Unknown error";
    }
    std::cerr << std::endl;
}
void Server::errorFcntl(int status)
{
    std::map<int, std::string> fcntl_errors;
        fcntl_errors[EBADF]  =   "The file descriptor is not valid.";
        fcntl_errors[EACCES] = "The operation is prohibited by locks that other processes have on the file.";
        fcntl_errors[EAGAIN] =  "The operation is prohibited by locks that other processes have on the file.";
        fcntl_errors[EDEADLK]=   "The operation would cause a deadlock.";
        fcntl_errors[EMFILE] =  "The process already has the maximum number of file descriptors open and tried to open more.";
        fcntl_errors[ENOLCK] =  "There are too many segment locks open, lock tables are full, or a remote locking protocol failed to satisfy a request.";
        fcntl_errors[EINVAL] =  "The third argument, cmd, is not valid, or the argument is F_SETLK or F_SETLKW and the data pointed to by lock is not valid.";
        fcntl_errors[EINTR]  = "The function was interrupted by a signal.";
        fcntl_errors[EIO]    =  "An I/O error occurred.";
        fcntl_errors[EFAULT] =  "lock points outside the process's allocated address space.";
        fcntl_errors[ENOTTY] =  "The specified file does not support the requested command.";
   std::cerr << "fcntl error: ";
    std::map<int, std::string>::const_iterator it = fcntl_errors.find(status);
    if (it != fcntl_errors.end()) {
        std::cerr << it->second;
    } else {
        std::cerr << "Unknown error";
    }
    std::cerr << std::endl;
}

void Server::errorSocketBinding(int status)
{
    std::map<int, std::string> bind_errors;
    bind_errors[EACCES] = "The address is protected, and the user is not the superuser.";
    bind_errors[EADDRINUSE] = "The given address is already in use.";
    bind_errors[EBADF] = "The file descriptor is not a valid file descriptor.";
    bind_errors[EINVAL] = "The socket is already bound to an address, or the addrlen is wrong, or the socket was not in the AF_UNIX family.";
    bind_errors[ENOTSOCK] = "The file descriptor sockfd does not refer to a socket.";
    bind_errors[EADDRNOTAVAIL] = "The specified address is not available from the local machine.";
    bind_errors[EFAULT] = "The addr argument is not in a writable part of the user address space.";

       std::cerr << "bind error: ";
    std::map<int, std::string>::const_iterator it = bind_errors.find(status);
    if (it != bind_errors.end()) {
        std::cerr << it->second;
    } else {
        std::cerr << "Unknown error";
    }
    std::cerr << std::endl;
}
void Server::errorListen(int status)
{
    std::map<int, std::string> listen_errors;
    listen_errors[EADDRINUSE] = "Another socket is already listening on the same port.";
    listen_errors[EBADF] = "The socket is not a valid file descriptor.";
    listen_errors[ENOTSOCK] = "The file descriptor sockfd does not refer to a socket.";
    listen_errors[EOPNOTSUPP] = "The socket is not of a type that supports the listen() operation.";
    listen_errors[EINVAL] = "The socket is already connected.";
    listen_errors[EACCES] = "The calling process does not have the appropriate privileges to listen on the specified port.";
    listen_errors[EINVAL] = "The socket is not bound to an address.";

      std::cerr << "listen error: ";
    std::map<int, std::string>::const_iterator it = listen_errors.find(status);
    if (it != listen_errors.end()) {
        std::cerr << it->second;
    } else {
        std::cerr << "Unknown error";
    }
    std::cerr << std::endl;
}
void Server::errorPoll(int status)
{
    std::map<int, std::string> poll_errors;
    poll_errors[EBADF] = "The fd member of one of the elements of the fds array is negative.";
    poll_errors[EFAULT] = "The memory area pointed to by the events or revents members of one of the elements of the fds array is not accessible.";
    poll_errors[EINTR] = "The call was interrupted by a signal handler before any of the requested events occurred or the timeout expired.";
    poll_errors[EINVAL] = "The nfds value exceeds the RLIMIT_NOFILE value.";
    poll_errors[EINVAL] = "The timeout value is invalid (less than -1).";

    std::cerr << "poll error: ";
    std::map<int, std::string>::const_iterator it = poll_errors.find(status);
    if (it != poll_errors.end()) {
        std::cerr << it->second;
    } else {
        std::cerr << "Unknown error";
    }
    std::cerr << std::endl;
}


