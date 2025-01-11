#ifndef OPERATIONSERVICE_HPP
#define OPERATIONSERVICE_HPP
#include <json.hpp>

#include "../types.hpp"
#include "../repository/Repository.hpp"


class OperationService {
    std::shared_ptr<Repository> repository_;

    /**
     *
     * @param payload - { username: <username> }
     * @param user - pointer to the user we want to add
     * @return - {}
     */
    ServiceResponse user_login(const nlohmann::json &payload, User *user) const;

    /**
     *
     * @param payload - { task_list_name: <tasklist_name> }
     * @return - { tasks: Task[] }
     */
    ServiceResponse get_all_tasks(const nlohmann::json &payload, const std::string &username) const;

    /**
     *
     * @param payload - { task_list_name: <tasklist_name>, task_name: <task_name>, task_description: <task_description> }
     * @return - {}
     */
    ServiceResponse create_task(const nlohmann::json &payload, const std::string &username) const;


    /**
     *
     * @param payload - { task_id: <task_id>, task_list_name: <task_list_name> }
     * @return - {}
     */
    ServiceResponse remove_task(const nlohmann::json &payload, const std::string &username) const;

    /**
    * @param username of connected and signed user, payload must be {}
    * @return { lists: [<list_name1>, <list_name2>] }
    */
    ServiceResponse get_all_user_task_lists(const std::string &username) const;

    /**
    * @param payload - { name: <task_list_name> }
    * @return status
    */
    ServiceResponse create_task_list(const nlohmann::json &payload, const std::string &username) const;

    /**
     * @param payload - { name: <task_list_name> }
     * @return status
     */
    ServiceResponse join_task_list(const nlohmann::json &payload, const std::string &username) const;

    static ServiceResponse handle_error(const std::string &error_message, const std::string &source);

public:
    explicit OperationService(std::shared_ptr<Repository> repository);

    ServiceResponse service_gateway(ResourceMethod resource_method, const nlohmann::json &payload, User *user) const;
};


#endif //OPERATIONSERVICE_HPP
