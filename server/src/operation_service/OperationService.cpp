#include "OperationService.hpp"

#include <utility>

OperationService::OperationService(std::shared_ptr<Repository> repository) {
    repository_ = std::move(repository);
}


ServiceResponse OperationService::service_gateway(const ResourceMethod resource_method, const nlohmann::json &payload) {
    switch (resource_method) {
        case AUTH_LOGIN: {
            break;
        }
        case T_GET_ALL: {
            break;
        }
        case T_CREATE: {
            break;
        }
        case T_DELETE: {
            break;
        }
        case TL_GET_ALL: {
            break;
        }
        case TL_CREATE: {
            break;
        }
        case TL_JOIN: {
            break;
        }
    }

    return {};
}

ServiceResponse user_login(const nlohmann::json &payload) {
    return {};
}
