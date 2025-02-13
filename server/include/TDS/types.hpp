#ifndef TYPES_HPP
#define TYPES_HPP

#include <queue>
#include <unordered_set>

#include <sys/socket.h>

#include <json.hpp>

// -= Resources =-
struct Task {
    int id;
    std::string title;
    std::string description;
};

struct User {
    int fd;
    std::string address;
    std::string port;

    std::string buffer{};

    std::string username; // unique
    std::queue<std::shared_ptr<Task>> task_notification_queue;

    ~User() {
        shutdown(fd, SHUT_RDWR);
        close(fd);
    }
};

struct TaskList {
    std::string name; // unique
    std::unordered_map<int, std::shared_ptr<Task>> tasks;
    std::unordered_set<std::string> shared_users;
};


// -= Parser =-
enum ResourceMethod {
    // Auth
    AUTH_LOGIN,
    // Task
    T_GET_ALL,
    T_CREATE,
    T_DELETE,
    // Task list
    TL_GET_ALL,
    TL_CREATE,
    TL_JOIN
};

struct ParserOutput {
    ResourceMethod resource_method;
    nlohmann::json payload;
};

struct Notification {
    std::vector<int> fds;
    std::string message;
};

struct ServiceResponse {
    std::string message;
    std::optional<Notification> notification;
};

enum FieldType {
    BOOLEAN,
    INTEGER,
    STRING
};

struct ValidatorFieldData {
    std::string field_name;
    FieldType field_type;
};

#endif //TYPES_HPP