#include "Parser.hpp"

#include "../exceptions.hpp"
#include "../types.hpp"

Parser::Parser() {
    request_method_map["T|GET_ALL"] = T_GET_ALL;
    request_method_map["T|CREATE"] = T_CREATE;
    request_method_map["T|DELETE"] = T_DELETE;

    request_method_map["TL|GET_ALL"] = TL_GET_ALL;
    request_method_map["TL|CREATE"] = TL_CREATE;
    request_method_map["TL|JOIN"] = TL_JOIN;
}

Parser::~Parser() = default;

std::vector<std::string> Parser::split(const std::string &s, const std::string &delimiter) const {
    size_t pos_start = 0, pos_end;
    const size_t delim_len = delimiter.length();
    std::vector<std::string> res;

    while ((pos_end = s.find(delimiter, pos_start)) != std::string::npos) {
        std::string token = s.substr(pos_start, pos_end - pos_start);
        pos_start = pos_end + delim_len;
        res.push_back(token);
    }

    res.push_back(s.substr(pos_start));
    return res;
}

bool Parser::is_request_valid(const std::size_t size) const {
    return size == 2;
}

ResourceMethod Parser::determine_method(const std::string &s) const {
    try {
        return request_method_map.at(s);
    } catch (const std::out_of_range &e) {
        throw parser_error("Method unknown");
    }
}

nlohmann::json Parser::parse_json(const std::string &s) const {
    try {
        return nlohmann::json::parse(s);
    } catch (const nlohmann::json::parse_error &e) {
        throw parser_error("Invalid request body");
    }
}

/*
*   EXPECTED:
*   AUTH|LOGIN
*   { "username": "<username>" }
*/
ParserOutput Parser::auth_request(const std::string &data) const {
    const auto lines = split(data, "\n");

    if (!is_request_valid(lines.size())) {
        throw parser_error("Request form invalid");
    }

    if (lines.at(0) != "AUTH|LOGIN") {
        throw parser_error("Operation not allowed");
    }

    ParserOutput output{
        .resource_method = AUTH_LOGIN,
        .payload = parse_json(lines.at(1))
    };

    return output;
}

/*
*   EXPECTED:
*   Method from ResourceMethod
*   Data in valid JSON
*/
ParserOutput Parser::process_request(const std::string &data) const {
    const auto lines = split(data, "\n");

    if (!is_request_valid(lines.size())) {
        throw parser_error("Request form invalid");
    }

    ParserOutput output = {
        .resource_method = determine_method(lines.at(0)),
        .payload = parse_json(lines.at(1))
    };

    return output;
}

std::vector<std::string> Parser::process_buffer(const std::string &buffer) const {
    return split(buffer, "\n\n");
}