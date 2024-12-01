#include <iostream>

int main(const int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <port>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    std::cout << "Hello from server at port " << argv[1] << std::endl;
    exit(EXIT_SUCCESS);
}