#ifndef PARSER_HPP
#define PARSER_HPP

#include "../types.hpp"

class Parser {
    std::unordered_map<std::string, ResourceMethod> request_method_map;

    std::vector<std::string> split(const std::string& s, const std::string& delimiter);
    bool is_request_valid(int size);
    ResourceMethod determine_method(const std::string& s);
    nlohmann::json parse_json(const std::string& s);
public:
    Parser();
    ~Parser();
    ParserOutput process_request(const std::string& data);
    ParserOutput auth_request(const std::string& data);
};

#endif //PARSER_HPP
