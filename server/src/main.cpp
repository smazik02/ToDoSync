#include <csignal>
#include <iostream>

#include "exceptions.hpp"
#include "server/Server.hpp"

int main(const int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <port>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    const int port = std::stoi(argv[1], nullptr, 10);

    std::printf("Hello from server at port %d\nIf you want to quit, enter 'q' or 'Q' on the console\n", port);

    try {
        auto server = Server(port);
        server.run();
    } catch (server_error &error) {
        perror(error.what());
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
