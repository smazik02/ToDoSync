#ifndef REPOSITORY_HPP
#define REPOSITORY_HPP

#include <map>

#include "../types.hpp"


class Repository {
    std::map<std::string, std::shared_ptr<User> > users;
    std::map<std::string, std::shared_ptr<TaskList> > task_lists;

public:
    std::shared_ptr<User> get_user_by_username(const std::string &username);

    void add_user(std::shared_ptr<User> user);

    void remove_user(const std::string &username);

    std::vector<std::shared_ptr<TaskList> > get_all_task_lists();

    std::shared_ptr<TaskList> get_task_list_by_name(const std::string &task_list_name);

    std::vector<std::shared_ptr<TaskList> > get_task_list_by_user(const std::string &username);

    void insert_task_list(std::shared_ptr<TaskList> task_list);
};


#endif //REPOSITORY_HPP
