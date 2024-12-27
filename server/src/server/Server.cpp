#include <netdb.h>
#include <unistd.h>

#include "Server.hpp"

#include <iostream>
#include <ranges>

#include "../exceptions.hpp"
#include "../types.hpp"

Server::Server(const int port): repository_(std::make_shared<Repository>()), parser_(Parser()),
                                operation_service_(OperationService(repository_)) {
    socket_fd = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (socket_fd == -1)
        throw server_error("socket create error");

    constexpr int one = 1;
    setsockopt(socket_fd, SOL_SOCKET, SO_REUSEADDR, &one, sizeof(one));

    address = {
        .sin_family = AF_INET,
        .sin_port = htons(port),
        .sin_addr = {.s_addr = INADDR_ANY}
    };
    address_len = sizeof(address);

    if (bind(socket_fd, reinterpret_cast<sockaddr *>(&address), address_len) == -1)
        throw server_error("bind error");

    if (listen(socket_fd, SOMAXCONN) == -1)
        throw server_error("listen error");

    epoll_fd = epoll_create1(0);
    if (epoll_fd == -1)
        throw server_error("epoll create error");

    server_ptr_ = std::make_unique<User>(socket_fd);
    events = {.events = EPOLLIN, .data = {.ptr = server_ptr_.get()}};
    epoll_ctl(epoll_fd, EPOLL_CTL_ADD, socket_fd, &events);

    const auto input = new User{.fd = STDIN_FILENO};
    events = {.events = EPOLLIN, .data = {.ptr = input}};
    epoll_ctl(epoll_fd, EPOLL_CTL_ADD, STDIN_FILENO, &events);
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
            throw server_error("epoll wait error");

        const auto incoming = static_cast<User *>(events.data.ptr);

        if (incoming->fd == STDIN_FILENO) {
            std::string input;
            std::cin >> input;
            if (input == "Q" || input == "q") {
                delete incoming;
                return;
            }
            continue;
        }

        if (incoming->fd == socket_fd) {
            const int user_fd = accept(incoming->fd, reinterpret_cast<sockaddr *>(&user_addr), &user_addr_len);
            if (user_fd == -1)
                throw server_error("accept error");

            const auto user = new User{.fd = user_fd};
            // const auto user = std::make_shared<User>(user_fd, "", "", ""); debug leftover
            repository_->append_not_logged(user_fd);

            epoll_event ee = {.events = EPOLLIN, .data = {.ptr = user}};
            epoll_ctl(epoll_fd, EPOLL_CTL_ADD, user_fd, &ee);

            getnameinfo(reinterpret_cast<sockaddr *>(&user_addr), user_addr_len, user->address.data(), NI_MAXHOST,
                        user->port.data(), NI_MAXSERV, 0);
            std::printf("New connection from %s:%s (fd: %d)\n", user->address.c_str(), user->port.c_str(), user->fd);
            continue;
        }

        char buffer[4096]{};
        const ssize_t cnt = recv(incoming->fd, buffer, 4095, 0);
        if (cnt < 1) {
            // TODO: handle error in a better way
            std::printf("Removing %d\n", incoming->fd);
            epoll_ctl(epoll_fd, EPOLL_CTL_DEL, incoming->fd, nullptr);
            repository_->remove_not_logged(incoming->fd);
            repository_->remove_user(incoming->username);
            continue;
        }

        incoming->buffer += std::string(buffer);

        auto messages = parser_.process_buffer(incoming->buffer);

        incoming->buffer = messages.at(messages.size() - 1);
        messages.pop_back();

        for (const auto &message: messages) {
            try {
                auto [resource_method, payload] = parser_.process_request(message);
                auto [response_message, notifications] = operation_service_.service_gateway(
                    resource_method, payload, incoming);
                if (notifications.has_value()) {
                    // TODO: execute notifications
                }

                send(incoming->fd, response_message.data(), response_message.size(), 0);
                std::printf("Response sent to client\n");
            } catch (parser_error &error) {
                std::printf("Parser error occurred\n");
                std::string error_msg = error.what();
                send(incoming->fd, error_msg.data(), error_msg.length(), 0);
                std::printf("Error message sent to client\n");
            }

            std::printf("%s", message.c_str());
        }
    }
}
