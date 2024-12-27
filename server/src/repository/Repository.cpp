#include "Repository.hpp"

#include <ranges>

bool Repository::is_user_logged(int fd) {
    return !not_logged_users.contains(fd);
}

void Repository::append_not_logged(int fd) {
    not_logged_users.insert(fd);
}

void Repository::remove_not_logged(int fd) {
    not_logged_users.erase(fd);
}

bool Repository::is_username_taken(const std::string &username) const {
    return users.contains(username);
}

std::optional<std::shared_ptr<User> > Repository::get_user_by_username(const std::string &username) {
    try {
        return users.at(username);
    } catch (std::out_of_range &e) {
        return std::nullopt;
    }
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

std::optional<std::shared_ptr<TaskList> > Repository::get_task_list_by_name(const std::string &task_list_name) {
    try {
        return task_lists.at(task_list_name);
    } catch (std::out_of_range &e) {
        return std::nullopt;
    }
}

bool Repository::is_task_list_name_taken(const std::string &task_list_name) const {
    return task_lists.contains(task_list_name);
}

std::vector<std::shared_ptr<TaskList> > Repository::get_task_list_by_user(const std::string &username) const {
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

int Repository::task_sequence_nextval() {
    return task_sequence++;
}
