#ifndef OPERATIONSERVICE_HPP
#define OPERATIONSERVICE_HPP
#include <json.hpp>

#include "../types.hpp"
#include "../repository/Repository.hpp"


class OperationService {
    std::shared_ptr<Repository> repository_;

public:
    explicit OperationService(std::shared_ptr<Repository> repository);

    ServiceResponse service_gateway(ResourceMethod resource_method, const nlohmann::json &payload, User *user) const;

    /**
     *
     * @param payload - { username: "username" }
     * @param user - pointer to the user we want to add
     * @return - {}
     */
    ServiceResponse user_login(const nlohmann::json &payload, User *user) const;

    /**
     * 
     * @param payload - { username: "username", task_list_name: "tasklist name" }
     * @return - { tasks: Task[] }
     */
    ServiceResponse get_all_tasks(const nlohmann::json &payload) const;

    /**
     * 
     * @param payload - { username: "username", task_list_name: "tasklist name", task_name: "task name", task_description: "task description" }
     * @return - {}
     */
    ServiceResponse create_task(const nlohmann::json &payload) const;


    /**
     * 
     * @param payload - { username: "username", task_id: "task id", task_list_name: "task list name" }
     * @return - {}
     */
    ServiceResponse remove_task(const nlohmann::json &payload) const;
};


#endif //OPERATIONSERVICE_HPP
