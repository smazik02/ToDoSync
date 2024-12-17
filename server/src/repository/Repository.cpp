#include "Repository.hpp"

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

std::vector<std::shared_ptr<TaskList> > Repository::get_all_task_lists() {
    std::vector<std::shared_ptr<TaskList> > return_list;
    for (const auto &task_list: task_lists | std::ranges::views::values) {
        return_list.push_back(task_list);
    }
    return return_list;
}

std::shared_ptr<TaskList> Repository::get_task_list_by_name(const std::string &task_list_name) {
    return task_lists[task_list_name];
}

std::vector<std::shared_ptr<TaskList> > Repository::get_task_list_by_user(const std::string &username) {
    std::vector<std::shared_ptr<TaskList> > return_list;
    for (const auto &task_list: task_lists | std::ranges::views::values) {
        const auto matches_username = [username](const auto &user) { return user->username == username; };
        const auto matching_user_iter = std::ranges::find_if(task_list->shared_users.begin(),
                                                             task_list->shared_users.end(),
                                                             matches_username);
        if (matching_user_iter != task_list->shared_users.end())
            return_list.push_back(task_list);
    }
    return return_list;
}

void Repository::insert_task_list(std::shared_ptr<TaskList> task_list) {
    task_lists.emplace(task_list->name, task_list);
}
