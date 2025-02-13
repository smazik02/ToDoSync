#include <TDS/Server.hpp>

#include <iostream>

#include <netdb.h>

#include <TDS/exceptions.hpp>
#include <TDS/Parser.hpp>

Server::Server(const int port): repository_(std::make_shared<Repository>()),
                                operation_service_(
                                    OperationService(repository_)) {
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

    if (bind(socket_fd, reinterpret_cast<sockaddr*>(&address),
             address_len) == -1)
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
        if (epoll_wait(epoll_fd, &events, 1, -1) == -1)
            throw server_error("epoll wait error");

        const auto incoming = static_cast<User*>(events.data.ptr);

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
            sockaddr_in user_addr{};
            socklen_t user_addr_len = sizeof user_addr;

            const int user_fd = accept(incoming->fd,
                                       reinterpret_cast<sockaddr*>(&user_addr),
                                       &user_addr_len);
            if (user_fd == -1)
                throw server_error("accept error");

            const auto user = new User{.fd = user_fd};
            repository_->append_not_logged(user_fd);

            epoll_event ee = {.events = EPOLLIN, .data = {.ptr = user}};
            epoll_ctl(epoll_fd, EPOLL_CTL_ADD, user_fd, &ee);

            char host_buf[NI_MAXHOST]{};
            char port_buf[NI_MAXSERV]{};
            getnameinfo(reinterpret_cast<sockaddr*>(&user_addr), user_addr_len,
                        host_buf, NI_MAXHOST, port_buf,
                        NI_MAXSERV, 0);
            user->address = std::string(host_buf);
            user->port = std::string(port_buf);
            std::printf("New connection from %s:%s (fd: %d)\n",
                        user->address.c_str(), user->port.c_str(), user->fd);
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

        auto messages = Parser::process_buffer(incoming->buffer);

        incoming->buffer = messages.at(messages.size() - 1);
        messages.pop_back();

        for (const auto& message : messages) {
            try {
                auto [resource_method, payload] = Parser::process_request(
                    message);
                auto [response_message, notifications] = operation_service_.
                    service_gateway(
                        resource_method, payload, incoming);
                if (notifications.has_value()) {
                    for (const auto& fd : notifications.value().fds) {
                        send(fd, notifications.value().message.c_str(),
                             notifications.value().message.size(), 0);
                    }
                }

                send(incoming->fd, response_message.c_str(),
                     response_message.size(), 0);
                std::printf("Response sent to client\n");
            } catch (parser_error& error) {
                std::printf("Parser error occurred\n");
                std::string error_msg = error.what();
                send(incoming->fd, error_msg.c_str(), error_msg.length(), 0);
                std::printf("Error message sent to client\n");
            }

            std::printf("%s\n", message.c_str());
        }
    }
}