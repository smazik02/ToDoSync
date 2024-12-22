#ifndef PARSER_HPP
#define PARSER_HPP

#include "../types.hpp"

class Parser {
    std::unordered_map<std::string, ResourceMethod> request_method_map;

    std::vector<std::string> split(const std::string &s, const std::string &delimiter) const;

    bool is_request_valid(std::size_t size) const;

    ResourceMethod determine_method(const std::string &s) const;

    nlohmann::json parse_json(const std::string &s) const;

public:
    Parser();

    ~Parser();

    ParserOutput process_request(const std::string &data) const;

    ParserOutput auth_request(const std::string &data) const;
};

#endif //PARSER_HPP
