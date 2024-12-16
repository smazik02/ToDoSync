#ifndef TYPES_HPP
#define TYPES_HPP

#include <map>
#include <set>
#include <string>
#include <queue>

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
    std::queue<Task *> task_notification_queue;
};

struct TaskList {
    std::string name; // unique
    std::map<int, Task *> tasks;
    std::set<User *> shared_users;
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
    std::string data;
};


#endif //TYPES_HPP
