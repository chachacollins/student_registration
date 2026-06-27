#pragma once
namespace utils
{

    using json = nlohmann::json;
    struct FieldRule
    {
        std::string name;
        json::value_t type;
    };

    std::optional<crow::response> validate_json_body(
            const crow::request& req,
            json& out,
            const std::vector<FieldRule>& rules
     );
}
