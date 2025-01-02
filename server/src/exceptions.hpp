#ifndef EXCEPTIONS_HPP
#define EXCEPTIONS_HPP

#include <exception>
#include <json.hpp>

class server_error final : public std::exception {
    const char *message;

public:
    explicit server_error(const char *message): message(message) {
    }

    const char *what() {
        return message;
    }
};

class parser_error final : public std::exception {
    const char *message;

public:
    explicit parser_error(const char *message): message(message) {
    }

    std::string what() {
        nlohmann::json body;
        body["message"] = message;
        return "FAIL\n" + body.dump() + "\n\n";
    }
};

class validator_error final : public std::exception {
    std::string message;

public:
    explicit validator_error(const std::string &message): message(message) {
    }

    std::string &what() {
        return message;
    }
};

#endif //EXCEPTIONS_HPP
