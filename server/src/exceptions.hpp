#ifndef EXCEPTIONS_HPP
#define EXCEPTIONS_HPP

#include <exception>

class server_error final : public std::exception {
    const char *message;

public:
    explicit server_error(const char *message): message(message) {
    }

    const char *what() {
        return message;
    }
};

#endif //EXCEPTIONS_HPP
