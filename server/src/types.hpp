#ifndef TYPES_HPP
#define TYPES_HPP

#include <sys/socket.h>

#include <map>
#include <memory>
#include <set>
#include <string>
#include <queue>
#include "../include/json.hpp"

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

    std::string username; // unique
    std::queue<std::shared_ptr<Task> > task_notification_queue;

    ~User() {
        shutdown(fd, SHUT_RDWR);
        close(fd);
    }
};

struct TaskList {
    std::string name; // unique
    std::map<int, std::shared_ptr<Task> > tasks;
    std::set<std::shared_ptr<User> > shared_users;
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

#endif //TYPES_HPP
