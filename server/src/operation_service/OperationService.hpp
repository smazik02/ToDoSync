#ifndef OPERATIONSERVICE_HPP
#define OPERATIONSERVICE_HPP
#include <json.hpp>

#include "../types.hpp"
#include "../repository/Repository.hpp"


class OperationService {
    std::shared_ptr<Repository> repository_;

public:
    explicit OperationService(std::shared_ptr<Repository> repository);

    ServiceResponse service_gateway(ResourceMethod resource_method, const nlohmann::json &payload);

    ServiceResponse user_login(const nlohmann::json &payload);
};


#endif //OPERATIONSERVICE_HPP
