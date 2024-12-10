#ifndef SERVER_HPP
#define SERVER_HPP

#include <arpa/inet.h>
#include <sys/epoll.h>
#include <sys/socket.h>

#include <string>

class Server {
public:
    Server(const std::string &port);

    void run();
    void stop();
private:
    int socket_fd;
    int epoll_fd;
    sockaddr_in address;
    socklen_t address_len;
    epoll_event events;
};



#endif //SERVER_HPP
