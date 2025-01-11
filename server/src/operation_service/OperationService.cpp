#include "OperationService.hpp"

#include <utility>

#include "../validator/Validator.hpp"
#include "../exceptions.hpp"

OperationService::OperationService(std::shared_ptr<Repository> repository) {
    repository_ = std::move(repository);
}

ServiceResponse OperationService::service_gateway(const ResourceMethod resource_method, const nlohmann::json &payload,
                                                  User *user) const {
    if (!repository_->is_user_logged(user->fd) && resource_method != AUTH_LOGIN) {
        return handle_error("Not logged", "AUTH");
    }

    if (repository_->is_user_logged(user->fd) && resource_method == AUTH_LOGIN) {
        return handle_error("You are already logged in", "AUTH");
    }

    switch (resource_method) {
        case AUTH_LOGIN: {
            return user_login(payload, user);
        }
        case T_GET_ALL: {
            return get_all_tasks(payload, user->username);
        }
        case T_CREATE: {
            return create_task(payload, user->username);
        }
        case T_DELETE: {
            return remove_task(payload, user->username);
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
    const std::vector<ValidatorFieldData> validator_data = {{"username", STRING}};
    try {
        Validator::validate(payload, validator_data);
    } catch (validator_error &e) {
        return handle_error(e.what(), "AUTH");
    }

    auto username = payload.at("username").get<std::string>();
    if (repository_->is_username_taken(username)) {
        return handle_error("User with that name already exists", "AUTH");
    }

    user->username = username;
    repository_->remove_not_logged(user->fd);
    repository_->add_user(std::shared_ptr<User>(user));
    return {.message = "OK\n{}\n\n", .notification = std::nullopt};
}

ServiceResponse OperationService::get_all_tasks(const nlohmann::json &payload, const std::string &username) const {
    const std::vector<ValidatorFieldData> validator_data = {{"task_list_name", STRING}};
    try {
        Validator::validate(payload, validator_data);
    } catch (validator_error &e) {
        return handle_error(e.what(), "T");
    }

    const auto task_list_name = payload.at("task_list_name").get<std::string>();
    const auto task_list = repository_->get_task_list_by_name(task_list_name);
    if (!task_list.has_value()) {
        return handle_error("Task list with that name doesn't exist", "T");
    }

    const auto user = repository_->get_user_by_username(username);
    if (!task_list.value()->shared_users.contains(user.value())) {
        return handle_error("You don't belong to this task list!", "T");
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

ServiceResponse OperationService::create_task(const nlohmann::json &payload, const std::string &username) const {
    const std::vector<ValidatorFieldData> validator_data = {
        {"task_list_name", STRING}, {"task_name", STRING}, {"task_description", STRING}
    };
    try {
        Validator::validate(payload, validator_data);
    } catch (validator_error &e) {
        return handle_error(e.what(), "T");
    }

    auto task = std::make_shared<Task>(
        repository_->task_sequence_nextval(),
        payload.at("task_name").get<std::string>(),
        payload.at("task_description").get<std::string>()
    );

    const auto task_list_name = payload.at("task_list_name").get<std::string>();
    const auto task_list = repository_->get_task_list_by_name(task_list_name);
    if (!task_list.has_value()) {
        return handle_error("Task list with that name doesn't exist", "T");
    }

    const auto user = repository_->get_user_by_username(username);
    if (!task_list.value()->shared_users.contains(user.value())) {
        return handle_error("You don't belong to this task list!", "T");
    }

    task_list.value()->tasks.emplace(task->id, task);

    std::optional<Notification> notification = std::nullopt;
    if (task_list.value()->shared_users.size() > 1) {
        nlohmann::json notification_json;
        notification_json["name"] = task_list_name;
        notification_json["description"] = "CREATE";

        const auto fds_size = task_list.value()->shared_users.size();
        notification = std::optional(Notification{.message = "NOTIFY\n" + notification_json.dump() + "\n\n"});
        notification.value().fds.reserve(fds_size);
        for (const auto &task_list_user: task_list.value()->shared_users) {
            if (task_list_user->fd != user.value()->fd) {
                notification.value().fds.push_back(task_list_user->fd);
            }
        }
    }

    auto response_json = nlohmann::json();
    response_json["source"] = "T";
    return {.message = "OK\n" + response_json.dump() + "\n\n", .notification = notification};
}

ServiceResponse OperationService::remove_task(const nlohmann::json &payload, const std::string &username) const {
    const std::vector<ValidatorFieldData> validator_data = {
        {"task_id", INTEGER}, {"task_list_name", STRING}
    };
    try {
        Validator::validate(payload, validator_data);
    } catch (validator_error &e) {
        return handle_error(e.what(), "T");
    }

    const auto task_id = payload.at("task_id").get<int>();
    const auto task_list_name = payload.at("task_list_name").get<std::string>();
    const auto task_list = repository_->get_task_list_by_name(task_list_name);

    if (!task_list.has_value()) {
        return handle_error("Task list with that name doesn't exist", "T");
    }

    const auto user = repository_->get_user_by_username(username);
    if (!task_list.value()->shared_users.contains(user.value())) {
        return handle_error("You don't belong to this task list!", "T");
    }

    task_list.value()->tasks.erase(task_id);

    std::optional<Notification> notification = std::nullopt;
    if (task_list.value()->shared_users.size() > 1) {
        nlohmann::json notification_json;
        notification_json["name"] = task_list_name;
        notification_json["description"] = "REMOVE";

        const auto fds_size = task_list.value()->shared_users.size();
        notification = std::optional(Notification{.message = "NOTIFY\n" + notification_json.dump() + "\n\n"});
        notification.value().fds.reserve(fds_size);
        for (const auto &task_list_user: task_list.value()->shared_users) {
            if (task_list_user->fd != user.value()->fd) {
                notification.value().fds.push_back(task_list_user->fd);
            }
        }
    }

    auto response_json = nlohmann::json();
    response_json["source"] = "T";
    return {.message = "OK\n" + response_json.dump() + "\n\n", .notification = notification};
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

    const std::vector<ValidatorFieldData> validator_data = {{"name", STRING}};
    try {
        Validator::validate(payload, validator_data);
    } catch (validator_error &e) {
        return handle_error(e.what(), "TL");
    }

    auto tl_name = payload.at("name").get<std::string>();

    if (repository_->is_task_list_name_taken(tl_name)) {
        return handle_error("Task list with that name already exists", "TL");
    }

    const auto task_list = std::make_shared<TaskList>(
        tl_name,
        std::unordered_map<int, std::shared_ptr<Task> >(),
        std::unordered_set({std::move(user)})
    );

    repository_->insert_task_list(task_list);

    auto response_json = nlohmann::json();
    response_json["source"] = "T";
    return {.message = "OK\n" + response_json.dump() + "\n\n", .notification = std::nullopt};
}

ServiceResponse OperationService::join_task_list(const nlohmann::json &payload, const std::string &username) const {
    const std::shared_ptr<User> user = repository_->get_user_by_username(username).value();

    const std::vector<ValidatorFieldData> validator_data = {{"name", STRING}};
    try {
        Validator::validate(payload, validator_data);
    } catch (validator_error &e) {
        return handle_error(e.what(), "TL");
    }

    const auto tl_name = payload.at("name").get<std::string>();
    const auto task_list = repository_->get_task_list_by_name(tl_name);
    if (!task_list.has_value()) {
        return handle_error("Task list with that name doesn't exist", "TL");
    }

    if (task_list.value()->shared_users.contains(user)) {
        return handle_error("User already belongs to the task list", "TL");
    }

    task_list.value()->shared_users.insert(user);

    auto response_json = nlohmann::json();
    response_json["source"] = "T";
    return {.message = "OK\n" + response_json.dump() + "\n\n", .notification = std::nullopt};
}

ServiceResponse OperationService::handle_error(const std::string &error_message, const std::string &source) {
    nlohmann::json error_json;
    error_json["message"] = error_message;
    error_json["source"] = source;
    return {.message = "FAIL\n" + error_json.dump() + "\n\n", .notification = std::nullopt};
}
