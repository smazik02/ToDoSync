#include "Server.hpp"

#include "../types.hpp"

Server::Server(const std::string &port) {
    socket_fd = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (socket_fd == -1) {
        perror("socket create error");
        exit(EXIT_FAILURE);
    }

    constexpr int one = 1;
    setsockopt(socket_fd, SOL_SOCKET, SO_REUSEADDR, &one, sizeof(one));

    address = {
        .sin_family = AF_INET,
        .sin_port = htons(atoi(port.c_str())),
        .sin_addr = { .s_addr = INADDR_ANY }
    };
    address_len = sizeof(address);

    if (bind(socket_fd, reinterpret_cast<sockaddr*>(&address), address_len) == -1) {
        perror("bind error");
        exit(EXIT_FAILURE);
    }

    if (listen(socket_fd, SOMAXCONN) == -1) {
        perror("bind error");
        exit(EXIT_FAILURE);
    }

    epoll_fd = epoll_create1(0);
    if (epoll_fd == -1) {
        perror("epoll create error");
        exit(EXIT_FAILURE);
    }

    const auto server = new User { .fd = socket_fd };
    events = { .events = EPOLLIN, .data = { .ptr = server } };
}


