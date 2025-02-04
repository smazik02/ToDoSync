#ifndef VALIDATOR_HPP
#define VALIDATOR_HPP

#include <json.hpp>

#include "../types.hpp"

class Validator {
public:
    static void validate(const nlohmann::json& input,
                         const std::vector<ValidatorFieldData>& validator_data);

    static const char* field_type_string(FieldType field_type);
};


#endif //VALIDATOR_HPP