#ifndef TYPES_HPP
#define TYPES_HPP

#include <string>
#include <vector>
#include <queue>

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
    std::queue<Task> task_notification_queue;
};

struct TaskList {
    std::string name; // unique
    std::vector<Task> tasks;
    std::vector<std::string> shared_users;
};

#endif //TYPES_HPP
