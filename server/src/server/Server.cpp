#include <netdb.h>
#include <unistd.h>

#include "Server.hpp"

#include <ranges>
#include <utility>

#include "../types.hpp"

Server::Server(const int port): repository_(std::make_shared<Repository>()) {
    socket_fd = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (socket_fd == -1)
        terminate("socket create error");

    constexpr int one = 1;
    setsockopt(socket_fd, SOL_SOCKET, SO_REUSEADDR, &one, sizeof(one));

    address = {
        .sin_family = AF_INET,
        .sin_port = htons(port),
        .sin_addr = {.s_addr = INADDR_ANY}
    };
    address_len = sizeof(address);

    if (bind(socket_fd, reinterpret_cast<sockaddr *>(&address), address_len) == -1)
        terminate("bind error");

    if (listen(socket_fd, SOMAXCONN) == -1)
        terminate("listen error");

    epoll_fd = epoll_create1(0);
    if (epoll_fd == -1)
        terminate("epoll create error");

    const auto server = new User{.fd = socket_fd};
    events = {.events = EPOLLIN, .data = {.ptr = server}};
    epoll_ctl(epoll_fd, EPOLL_CTL_ADD, socket_fd, &events);
}

Server::~Server() {
    close(epoll_fd);
    shutdown(socket_fd, SHUT_RDWR);
    close(socket_fd);
}


void Server::run() {
    while (true) {
        sockaddr_in user_addr{};
        socklen_t user_addr_len = sizeof user_addr;

        if (epoll_wait(epoll_fd, &events, 1, -1) == -1)
            terminate("epoll wait error");

        const auto incoming = static_cast<User *>(events.data.ptr);

        if (incoming->fd == socket_fd) {
            const int user_fd = accept(incoming->fd, reinterpret_cast<sockaddr *>(&user_addr), &user_addr_len);
            if (user_fd == -1)
                terminate("accept error");

            // const auto user = std::make_shared<User>(user_fd, "", "", std::to_string(user_fd));
            // repository_->add_user(user);
            const auto user = new User{.fd = user_fd};

            epoll_event ee = {.events = EPOLLIN, .data = {.ptr = user}};
            epoll_ctl(epoll_fd, EPOLL_CTL_ADD, user_fd, &ee);

            getnameinfo(reinterpret_cast<sockaddr *>(&user_addr), user_addr_len, user->address.data(), NI_MAXHOST,
                        user->port.data(), NI_MAXSERV, 0);
            std::printf("New connection from %s:%s (fd: %d)\n", user->address.c_str(), user->port.c_str(), user->fd);
            continue;
        }

        char buffer[4096]{};
        const ssize_t cnt = recv(incoming->fd, buffer, 4096, 0);

        if (cnt < 1) {
            std::printf("Removing %d\n", incoming->fd);
            epoll_ctl(epoll_fd, EPOLL_CTL_DEL, incoming->fd, nullptr);
            repository_->remove_user(incoming->username);
            continue;
        }

        std::printf("%s\n", buffer);
    }
}

void Server::stop() {
}

void Server::terminate(const char *message) {
    perror(message);
    close(epoll_fd);
    shutdown(socket_fd, SHUT_RDWR);
    close(socket_fd);
    exit(EXIT_FAILURE);
}
