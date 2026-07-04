#include "courses.hpp"

static std::string get_student_id(const crow::request& req)
{
    std::string myauth = req.get_header_value("Authorization");
    std::string token  = myauth.substr(7);
    auto decoded = jwt::decode(token);
    return decoded.get_payload_claim("student_id").as_string();
}

namespace routes
{
    crow::response get_courses(pqxx::connection& cx, const crow::request& req)
    {
        auto student_id = get_student_id(req);
        auto dept_code  = utils::split_string(student_id, "-")[0];
        if (dept_code.size() != 4)
        {
            return crow::response(400, json{{"error", 
                   std::format("malformed department code {}", dept_code)}}.dump());
        }
        std::vector<json> courses;
        pqxx::work tx{cx};
        try
        {
            pqxx::result rows{
                tx.exec("SELECT * FROM courses WHERE dept_code = $1", pqxx::params{dept_code})
            };

            for (const auto& row : rows)
            {
                json course;
                course["course_id"]      = row["course_id"].as<std::string>();
                course["dept_code"]      = row["dept_code"].as<std::string>();
                course["course_name"]    = row["course_name"].as<std::string>();
                course["description"]    = row["description"].is_null() ? "" : row["description"].as<std::string>();
                course["capacity"]       = row["capacity"].as<int>();
                course["enrolled_count"] = row["enrolled_count"].as<int>();
                course["semester"]       = row["semester"].as<std::string>();
                course["day_of_week"]    = row["day_of_week"].as<std::string>();
                course["start_time"]     = row["start_time"].as<std::string>();
                course["end_time"]       = row["end_time"].as<std::string>();
                course["room"]           = row["room"].is_null() ? "" : row["room"].as<std::string>();
                course["is_active"]      = row["is_active"].as<bool>();
                courses.emplace_back(course);
            }
            return crow::response(200, json{courses}.dump());
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

    crow::response register_course(pqxx::connection& cx, const crow::request& req, std::string course_id)
    {
        auto student_id = get_student_id(req);
        auto dept_code  = utils::split_string(student_id, "-")[0];
        pqxx::work tx{cx};
        try
        {
            pqxx::result rows{
                tx.exec("SELECT * FROM courses WHERE dept_code = $1 AND course_id = $2", pqxx::params{dept_code, course_id})
            };
            if (rows.empty())
            {
                return crow::response(404, json{{"error", "Course not found"}}.dump());
            }
            auto capacity = rows[0]["capacity"].as<int>();
            auto enrolled_count = rows[0]["enrolled_count"].as<int>();
            if (enrolled_count >= capacity)
            {
                return crow::response(400, json{{"error", "Course is full"}}.dump());
            }
            tx.exec("INSERT INTO registrations (student_id, course_id) VALUES ($1, $2)", pqxx::params{student_id, course_id});
            tx.commit();
            return crow::response(201, json{{"success", true}}.dump());
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
