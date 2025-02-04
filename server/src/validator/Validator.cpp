#include "Validator.hpp"

#include "../exceptions.hpp"

void Validator::validate(const nlohmann::json& input,
                         const std::vector<ValidatorFieldData>&
                         validator_data) {
    std::string validator_string;

    for (const auto& [field_name, field_type] : validator_data) {
        if (!input.contains(field_name)) {
            validator_string += "no " + field_name + " given, ";
            continue;
        }

        bool correct;
        switch (field_type) {
            case BOOLEAN: {
                correct = input.at(field_name).is_boolean();
                break;
            }
            case INTEGER: {
                correct = input.at(field_name).is_number_integer();
                break;
            }
            case STRING: {
                correct = input.at(field_name).is_string();
                break;
            }
            default:
                correct = false;
        }

        if (!correct)
            validator_string += "field " + field_name + " should be of type " +
                field_type_string(field_type) +
                ", ";
    }

    if (!validator_string.empty())
        throw validator_error(
            validator_string.substr(0, validator_string.size() - 2));
}

const char* Validator::field_type_string(const FieldType field_type) {
    switch (field_type) {
        case BOOLEAN:
            return "bool";
        case INTEGER:
            return "int";
        case STRING:
            return "string";
        default:
            return "?";
    }
}