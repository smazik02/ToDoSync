#ifndef SERVER_HPP
#define SERVER_HPP

#include <memory>
#include <unordered_map>

#include <sys/epoll.h>
#include <arpa/inet.h>

#include <TDS/OperationService.hpp>
#include <TDS/Repository.hpp>
#include <TDS/types.hpp>

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