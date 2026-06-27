#include "utils.hpp"
namespace utils
{
    std::optional<crow::response> validate_json_body(
            const crow::request& req,
            json& out,
            const std::vector<FieldRule>& rules
            )
    {
        if (req.body.empty())
            return crow::response(400, json{{"error", "request body is empty"}}.dump());

        try
        {
            out = json::parse(req.body);
        }
        catch (const json::parse_error& e)
        {
            return crow::response(400, json{{"error", e.what()}}.dump());
        }

        for (const auto& rule : rules) 
        {
            if (!out.contains(rule.name))
                return crow::response(400, json{{"error", "missing field: " + rule.name}}.dump());
            if (out[rule.name].type() != rule.type)
                return crow::response(400, json{{"error", "invalid type for field: " + rule.name}}.dump());
            if (rule.type == json::value_t::string && out[rule.name].get<std::string>().empty())
                return crow::response(400, json{{"error", "field cannot be empty: " + rule.name}}.dump());
        }
        return std::nullopt;
    }

    std::string jwt_create(const std::string_view& str)
    {
        auto token = jwt::create()
                         .set_issuer("auth0")
                         .set_type("JWS")
                         .set_payload_claim("student_id", jwt::claim(std::string(str)))
                         .sign(jwt::algorithm::hs256{"secret"});
        return token;
    }
}
