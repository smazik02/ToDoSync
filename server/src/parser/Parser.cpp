#include "Parser.hpp"

#include "../types.hpp"

std::vector<std::string> Parser::split(const std::string& s, const std::string& delimiter) {
    size_t pos_start = 0, pos_end, delim_len = delimiter.length();
    std::string token;
    std::vector<std::string> res;

    while ((pos_end = s.find(delimiter, pos_start)) != std::string::npos) {
        token = s.substr (pos_start, pos_end - pos_start);
        pos_start = pos_end + delim_len;
        res.push_back (token);
    }

    res.push_back (s.substr (pos_start));
    return res;
}

bool Parser::is_request_valid(const std::string request_data) {
    return split(request_data, "\n").size() == 2;
}

ParserOutput Parser::auth_request(const std::string& data) {
    /* TODO:
     *  - request validation
     *  - splitting two lines
     *  - get header and make enum
     *  - get json part and validate it by required keys
     */
    if (!is_request_valid(data)) {
        // throw exception
    }
    std::string method = data.substr(0, data.find('|'));
}

ParserOutput Parser::process_request(const std::string& data) {

}

