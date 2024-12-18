#include <iostream>

#include "server/Server.hpp"

int main(const int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <port>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    const int port = std::stoi(argv[1], nullptr, 10);

    std::cout << "Hello from server at port " << argv[1] << std::endl;

    auto server = Server(port);

    server.run();

    exit(EXIT_SUCCESS);
}
