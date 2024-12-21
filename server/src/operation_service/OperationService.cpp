#include "OperationService.hpp"

#include <utility>

OperationService::OperationService(std::shared_ptr<Repository> repository) {
    repository_ = std::move(repository);
}

ServiceResponse OperationService::service_gateway(const ResourceMethod resource_method, const nlohmann::json &payload,
                                                  User *user) {
    // TODO: validate username in payload
    switch (resource_method) {
        case AUTH_LOGIN: {
            return user_login(payload, user);
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

ServiceResponse OperationService::user_login(const nlohmann::json &payload, User *user) const {
    if (!payload.contains("username"))
        throw std::exception();

    std::string username = payload.at("username");
    if (repository_->is_username_taken(username))
        return {.message = "user with that name already exists", .notification = std::nullopt};

    repository_->add_user(std::shared_ptr<User>(user));
    return {.message = "success", .notification = std::nullopt};
}
