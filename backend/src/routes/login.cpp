#include "login.hpp"

namespace routes
{
    crow::response login(pqxx::connection& cx, const crow::request& req)
    {
        json body;
        if (auto err = utils::validate_json_body(req, body, {
                        {"email",      json::value_t::string},
                        {"password",   json::value_t::string}})
            ) return std::move(err.value());
        pqxx::work tx{cx};
        try
        {
            pqxx::result result{tx.exec(
                "select * from students where email = $1",
                pqxx::params{body["email"].get<std::string>()}
            )};

            if (result.empty())
                return crow::response(404, json{{"error", "user not found"}}.dump());

            auto row = result.front();
            auto user_password = row["password_hash"].c_str();
            std::string password = body["password"].get<std::string>();

            if (crypto_pwhash_str_verify(user_password, password.c_str(), password.size()) != 0)
                return crow::response(401, json{{"error", "wrong password"}}.dump());

            auto token = utils::jwt_create(row["student_id"].c_str());
            return crow::response(200, json{{"success", true}, {"token", token}}.dump());
        }
        catch (pqxx::failure const &e)
        {
            std::cerr << "SQL error: " << e.what() << '\n';
            return crow::response(500, json{{"error", "Internal Server Error"}}.dump());
        }
        catch (std::exception const &e)
        {
            return crow::response(409, json{{"error", e.what()}}.dump());
        }
    }
}
