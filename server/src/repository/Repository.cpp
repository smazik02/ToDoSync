#include "Repository.hpp"

#include <algorithm>
#include <ranges>

std::shared_ptr<User> Repository::get_user_by_username(const std::string &username) {
    return users[username];
}

void Repository::add_user(std::shared_ptr<User> user) {
    users.emplace(user->username, user);
}

void Repository::remove_user(const std::string &username) {
    users.erase(username);
}

void Repository::erase_users() {
    users.clear();
}

std::vector<std::shared_ptr<TaskList> > Repository::get_all_task_lists() {
    std::vector<std::shared_ptr<TaskList> > return_list;
    for (const auto &task_list: task_lists | std::ranges::views::values) {
        return_list.push_back(task_list);
    }
    return return_list;
}

std::vector<std::shared_ptr<TaskList> > Repository::get_task_list_by_user(const std::string &username) {
    std::vector<std::shared_ptr<TaskList> > return_list;
    for (const auto &task_list: task_lists | std::ranges::views::values) {
        for (const auto &user: task_list->shared_users) {
            if (user->username == username) {
                return_list.push_back(task_list);
                break;
            }
        }
    }
    return return_list;
}

void Repository::insert_task_list(std::shared_ptr<TaskList> task_list) {
    task_lists.emplace(task_list->name, task_list);
}
