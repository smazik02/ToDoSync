#include "Parser.hpp"

#include "../exceptions.hpp"
#include "../types.hpp"

std::vector<std::string> Parser::split(const std::string& s,
                                       const std::string& delimiter) {
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

bool Parser::is_request_valid(const std::size_t size) {
    return size == 2;
}

ResourceMethod Parser::determine_method(const std::string& s) {
    if (s == "AUTH|LOGIN") return AUTH_LOGIN;
    if (s == "T|GET_ALL") return T_GET_ALL;
    if (s == "T|CREATE") return T_CREATE;
    if (s == "T|DELETE") return T_DELETE;
    if (s == "TL|GET_ALL") return TL_GET_ALL;
    if (s == "TL|CREATE") return TL_CREATE;
    if (s == "TL|JOIN") return TL_JOIN;

    throw parser_error("Method unknown");
}

nlohmann::json Parser::parse_json(const std::string& s) {
    try {
        return nlohmann::json::parse(s);
    } catch (const nlohmann::json::parse_error& e) {
        throw parser_error("Invalid request body");
    }
}

/*
*   EXPECTED:
*   Method from ResourceMethod
*   Data in valid JSON
*/
ParserOutput Parser::process_request(const std::string& data) {
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

std::vector<std::string> Parser::process_buffer(const std::string& buffer) {
    return split(buffer, "\n\n");
}