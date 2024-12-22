#include "OperationService.hpp"

#include <utility>

OperationService::OperationService(std::shared_ptr<Repository> repository) {
    repository_ = std::move(repository);
}

ServiceResponse OperationService::service_gateway(const ResourceMethod resource_method, const nlohmann::json &payload,
                                                  User *user) const {
    // TODO: validate username in payload
    switch (resource_method) {
        case AUTH_LOGIN: {
            return user_login(payload, user);
        }
        case T_GET_ALL: {
            return get_all_tasks(payload);
        }
        case T_CREATE: {
            return create_task(payload);
        }
        case T_DELETE: {
            return remove_task(payload);
        }
        case TL_GET_ALL: {
            return get_all_user_task_lists(user->username);
        }
        case TL_CREATE: {
            return create_task_list(payload, user->username);
        }
        case TL_JOIN: {
            return join_task_list(payload, user->username);
        }
    }

    return {};
}

ServiceResponse OperationService::user_login(const nlohmann::json &payload, User *user) const {
    if (!payload.contains("username"))
        throw std::exception();
    if (!payload.at("username").is_string())
        throw std::exception();

    auto username = payload.at("username").get<std::string>();
    if (repository_->is_username_taken(username))
        return {.message = "user with that name already exists", .notification = std::nullopt};

    repository_->add_user(std::shared_ptr<User>(user));
    return {.message = "success", .notification = std::nullopt};
}

ServiceResponse OperationService::get_all_tasks(const nlohmann::json &payload) const {
    if (!payload.contains("task_list_name"))
        throw std::exception();
    if (!payload.at("task_list_name").is_string())
        throw std::exception();

    const auto task_list_name = payload.at("task_list_name").get<std::string>();
    const auto task_list = repository_->get_task_list_by_name(task_list_name);

    auto response_json = nlohmann::json();
    response_json["tasks"] = nlohmann::json::array();
    for (const auto &task: task_list->tasks | std::ranges::views::values) {
        auto task_to_add = nlohmann::json();
        task_to_add["id"] = task->id;
        task_to_add["title"] = task->title;
        task_to_add["description"] = task->description;
        response_json["tasks"].push_back(task_to_add);
    }

    return {.message = response_json.dump(), .notification = std::nullopt};
}

ServiceResponse OperationService::create_task(const nlohmann::json &payload) const {
    if (!payload.contains("task_list_name") || !payload.contains("task_name") || !payload.contains("task_description"))
        throw std::exception();
    if (!payload.at("task_list_name").is_string() || !payload.at("task_name").is_string() ||
        !payload.at("task_description").is_string())
        throw std::exception();

    auto task = std::make_shared<Task>(
        repository_->task_sequence_nextval(),
        payload.at("task_name").get<std::string>(),
        payload.at("task_description").get<std::string>()
    );

    const auto task_list_name = payload.at("task_list_name").get<std::string>();
    repository_->get_task_list_by_name(task_list_name)->tasks.emplace(task->id, task);

    return {.message = "success", .notification = std::nullopt};
}

ServiceResponse OperationService::remove_task(const nlohmann::json &payload) const {
    if (!payload.contains("task_id") || !payload.contains("task_list_name"))
        throw std::exception();
    if (!payload.at("task_id").is_number_integer() || !payload.at("task_list_name").is_string())
        throw std::exception();

    const auto task_id = payload.at("task_id").get<int>();
    const auto task_list_name = payload.at("task_list_name").get<std::string>();
    repository_->get_task_list_by_name(task_list_name)->tasks.erase(task_id);

    return {.message = "success", .notification = std::nullopt};
}

ServiceResponse OperationService::get_all_user_task_lists(const std::string &username) const {
    auto lists = repository_->get_task_list_by_user(username);

    nlohmann::json response;
    response["lists"] = nlohmann::json::array();

    for (const auto &list : lists) {
        response["lists"].push_back(list->name);
    }

    return {
        .message = response.dump(),
        .notification = std::nullopt
    };
}

ServiceResponse OperationService::create_task_list(const nlohmann::json &payload, const std::string &username) const {
    std::shared_ptr<User> user = repository_->get_user_by_username(username);

    if (!payload.contains("name")) {
        throw std::exception();
    }

    if (!payload.at("name").is_string()) {
        throw std::exception();
    }

    std::string tl_name = payload.at("name").get<std::string>();

    if (repository_->is_task_list_name_taken(tl_name)) {
        throw std::exception();
    }

    auto task_list = std::make_shared<TaskList>(
        tl_name,
        std::map<int, std::shared_ptr<Task>>(),
        std::set({ std::move(user) })
    );

    repository_->insert_task_list(task_list);

    return {
        .message = "status",
        .notification = std::nullopt
    };
}

ServiceResponse OperationService::join_task_list(const nlohmann::json &payload, const std::string &username) const {
    std::shared_ptr<User> user = repository_->get_user_by_username(username);

    if (!payload.contains("name")) {
        throw std::exception();
    }

    if (!payload.at("name").is_string()) {
        throw std::exception();
    }

    std::string tl_name = payload.at("name").get<std::string>();

    if (!repository_->is_task_list_name_taken(tl_name)) {
        throw std::exception();
    }

    auto task_list = repository_->get_task_list_by_name(tl_name);

    if (task_list->shared_users.contains(user)) {
        throw std::exception();
    }

    task_list->shared_users.insert(user);

    return {
        .message = "status",
        .notification = std::nullopt
    };
}
