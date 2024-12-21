#ifndef OPERATIONSERVICE_HPP
#define OPERATIONSERVICE_HPP
#include <json.hpp>

#include "../types.hpp"
#include "../repository/Repository.hpp"


class OperationService {
    std::shared_ptr<Repository> repository_;

public:
    explicit OperationService(std::shared_ptr<Repository> repository);

    ServiceResponse service_gateway(ResourceMethod resource_method, const nlohmann::json &payload, User* user);

    /**
     *
     * @param payload - { username: "username" }
     * @param user - pointer to the user we want to add
     * @return - {}
     */
    ServiceResponse user_login(const nlohmann::json &payload, User* user) const;
};


#endif //OPERATIONSERVICE_HPP
