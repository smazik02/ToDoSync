#ifndef SERVER_HPP
#define SERVER_HPP

#include <arpa/inet.h>
#include <sys/epoll.h>
#include <sys/socket.h>

#include "../types.hpp"
#include "../repository/Repository.hpp"

class Server {
    std::shared_ptr<Repository> repository_;
    int socket_fd;
    int epoll_fd;
    sockaddr_in address{};
    socklen_t address_len;
    epoll_event events{};
    std::map<int, std::unique_ptr<User> > users;

public:
    Server(int port, std::shared_ptr<Repository> repository);

    ~Server();

    void run();

    void stop();

    void terminate(const char *message);
};


#endif //SERVER_HPP
