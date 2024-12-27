#include "OperationService.hpp"

#include <utility>

OperationService::OperationService(std::shared_ptr<Repository> repository) {
    repository_ = std::move(repository);
}

ServiceResponse OperationService::service_gateway(const ResourceMethod resource_method, const nlohmann::json &payload,
                                                  User *user) const {
    if (!repository_->is_user_logged(user->fd) && resource_method != AUTH_LOGIN) {
        return handle_error("Not logged");
    }

    if (repository_->is_user_logged(user->fd) && resource_method == AUTH_LOGIN) {
        return handle_error("You are already logged in");
    }

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
    if (!payload.contains("username") || !payload.at("username").is_string()) {
        return handle_error("JSON validation error");
    }

    auto username = payload.at("username").get<std::string>();
    if (repository_->is_username_taken(username)) {
        return handle_error("User with that name already exists");
    }

    repository_->remove_not_logged(user->fd);
    repository_->add_user(std::shared_ptr<User>(user));
    return {.message = "OK\n\n", .notification = std::nullopt};
}

ServiceResponse OperationService::get_all_tasks(const nlohmann::json &payload) const {
    if (!payload.contains("task_list_name") || !payload.at("task_list_name").is_string()) {
        return handle_error("JSON validation error");
    }

    const auto task_list_name = payload.at("task_list_name").get<std::string>();
    const auto task_list = repository_->get_task_list_by_name(task_list_name);
    if (!task_list.has_value()) {
        return handle_error("Task list with that name doesn't exist");
    }

    auto response_json = nlohmann::json();
    response_json["tasks"] = nlohmann::json::array();
    for (const auto &task: task_list.value()->tasks | std::ranges::views::values) {
        auto task_to_add = nlohmann::json();
        task_to_add["id"] = task->id;
        task_to_add["title"] = task->title;
        task_to_add["description"] = task->description;
        response_json["tasks"].push_back(task_to_add);
    }

    return {.message = "OK\n" + response_json.dump() + "\n\n", .notification = std::nullopt};
}

ServiceResponse OperationService::create_task(const nlohmann::json &payload) const {
    if (!payload.contains("task_list_name") || !payload.contains("task_name") ||
        !payload.contains("task_description")) {
        return handle_error("JSON validation error");
    }
    if (!payload.at("task_list_name").is_string() || !payload.at("task_name").is_string() ||
        !payload.at("task_description").is_string()) {
        return handle_error("JSON validation error");
    }

    auto task = std::make_shared<Task>(
        repository_->task_sequence_nextval(),
        payload.at("task_name").get<std::string>(),
        payload.at("task_description").get<std::string>()
    );

    const auto task_list_name = payload.at("task_list_name").get<std::string>();
    const auto task_list = repository_->get_task_list_by_name(task_list_name);
    if (!task_list.has_value()) {
        return handle_error("Task list with that name doesn't exist");
    }

    task_list.value()->tasks.emplace(task->id, task);

    return {.message = "OK\n\n", .notification = std::nullopt};
}

ServiceResponse OperationService::remove_task(const nlohmann::json &payload) const {
    if (!payload.contains("task_id") || !payload.contains("task_list_name")) {
        return handle_error("JSON validation error");
    }
    if (!payload.at("task_id").is_number_integer() || !payload.at("task_list_name").is_string()) {
        return handle_error("JSON validation error");
    }

    const auto task_id = payload.at("task_id").get<int>();
    const auto task_list_name = payload.at("task_list_name").get<std::string>();
    const auto task_list = repository_->get_task_list_by_name(task_list_name);

    if (!task_list.has_value()) {
        return handle_error("Task list with that name doesn't exist");
    }

    task_list.value()->tasks.erase(task_id);

    return {.message = "OK\n\n", .notification = std::nullopt};
}

ServiceResponse OperationService::get_all_user_task_lists(const std::string &username) const {
    auto lists = repository_->get_task_list_by_user(username);

    nlohmann::json response;
    response["lists"] = nlohmann::json::array();

    for (const auto &list: lists) {
        response["lists"].push_back(list->name);
    }

    return {
        .message = "OK\n" + response.dump() + "\n\n",
        .notification = std::nullopt
    };
}

ServiceResponse OperationService::create_task_list(const nlohmann::json &payload, const std::string &username) const {
    std::shared_ptr<User> user = repository_->get_user_by_username(username).value();

    if (!payload.contains("name") || !payload.at("name").is_string()) {
        return handle_error("JSON validation error");
    }

    auto tl_name = payload.at("name").get<std::string>();

    if (repository_->is_task_list_name_taken(tl_name)) {
        return handle_error("Task list with that name already exists");
    }

    const auto task_list = std::make_shared<TaskList>(
        tl_name,
        std::unordered_map<int, std::shared_ptr<Task> >(),
        std::unordered_set({std::move(user)})
    );

    repository_->insert_task_list(task_list);

    return {
        .message = "status",
        .notification = std::nullopt
    };
}

ServiceResponse OperationService::join_task_list(const nlohmann::json &payload, const std::string &username) const {
    const std::shared_ptr<User> user = repository_->get_user_by_username(username).value();

    if (!payload.contains("name") || !payload.at("name").is_string()) {
        return handle_error("JSON validation error");
    }

    const auto tl_name = payload.at("name").get<std::string>();
    const auto task_list = repository_->get_task_list_by_name(tl_name);
    if (!task_list.has_value()) {
        return handle_error("Task list with that name doesn't exist");
    }

    if (task_list.value()->shared_users.contains(user)) {
        return handle_error("User already belongs to the task list");
    }

    task_list.value()->shared_users.insert(user);

    return {
        .message = "OK\n\n",
        .notification = std::nullopt
    };
}

ServiceResponse OperationService::handle_error(const char *error_message) const {
    nlohmann::json error_json;
    error_json["message"] = error_message;
    return {.message = "FAIL\n" + error_json.dump() + "\n\n", .notification = std::nullopt};
}
