#ifndef REPOSITORY_HPP
#define REPOSITORY_HPP

#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>

#include <TDS/types.hpp>

class Repository {
    int task_sequence = 0;
    std::unordered_map<std::string, std::shared_ptr<User>> users;
    std::unordered_map<std::string, std::shared_ptr<TaskList>> task_lists;
    std::unordered_set<int> not_logged_users;

public:
    bool is_user_logged(int fd);

    void append_not_logged(int fd);

    void remove_not_logged(int fd);

    bool is_username_taken(const std::string& username) const;

    std::optional<std::shared_ptr<User>> get_user_by_username(
        const std::string& username);

    void add_user(std::shared_ptr<User> user);

    void remove_user(const std::string& username);

    std::vector<std::shared_ptr<TaskList>> get_all_task_lists();

    std::optional<std::shared_ptr<TaskList>> get_task_list_by_name(
        const std::string& task_list_name);

    bool is_task_list_name_taken(const std::string& task_list_name) const;

    std::vector<std::shared_ptr<TaskList>> get_task_list_by_user(
        const std::string& username) const;

    void insert_task_list(std::shared_ptr<TaskList> task_list);

    int task_sequence_nextval();
};


#endif //REPOSITORY_HPP