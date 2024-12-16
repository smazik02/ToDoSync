#ifndef PARSER_HPP
#define PARSER_HPP

#include "../types.hpp"

class Parser {
    std::vector<std::string> split(const std::string& s, const std::string& delimiter);
    bool is_request_valid(const std::string request_data);
public:
    Parser();
    ~Parser();
    ParserOutput process_request(const std::string& data);
    ParserOutput auth_request(const std::string& data);
};

#endif //PARSER_HPP
