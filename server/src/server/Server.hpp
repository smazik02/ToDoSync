#ifndef SERVER_HPP
#define SERVER_HPP

#include <arpa/inet.h>
#include <sys/epoll.h>
#include <sys/socket.h>

#include <string>

#include "../types.hpp"

class Server {
public:
    Server(const std::string &port);

    void run();

    void stop();

    void terminate(const char *message);

private:
    int socket_fd;
    int epoll_fd;
    sockaddr_in address{};
    socklen_t address_len;
    epoll_event events{};
    std::map<int, User *> users;
};


#endif //SERVER_HPP
