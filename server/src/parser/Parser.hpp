#ifndef PARSER_HPP
#define PARSER_HPP

#include "../types.hpp"

class Parser {
    static std::vector<std::string> split(const std::string &s, const std::string &delimiter);

    static bool is_request_valid(std::size_t size);

    static ResourceMethod determine_method(const std::string &s);

    static nlohmann::json parse_json(const std::string &s);

public:
    static ParserOutput process_request(const std::string &data);

    static std::vector<std::string> process_buffer(const std::string &buffer);
};

#endif //PARSER_HPP
