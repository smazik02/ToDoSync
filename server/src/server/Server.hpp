#ifndef SERVER_HPP
#define SERVER_HPP

#include <arpa/inet.h>
#include <sys/epoll.h>
#include <sys/socket.h>

#include "../types.hpp"
#include "../operation_service/OperationService.hpp"
#include "../repository/Repository.hpp"

class Server {
    std::shared_ptr<Repository> repository_;
    OperationService operation_service_;

    int socket_fd;
    int epoll_fd;
    std::unique_ptr<User> server_ptr_;
    sockaddr_in address{};
    socklen_t address_len;
    epoll_event events{};
    std::unordered_map<int, std::unique_ptr<User>> users;

public:
    explicit Server(int port);

    ~Server();

    void run();
};


#endif //SERVER_HPP