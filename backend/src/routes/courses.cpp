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
    crow::response get_course_by_id(pqxx::connection& cx, std::string course_id)
    {
        pqxx::work tx{cx};
        try
        {
            pqxx::result rows{tx.exec("SELECT * FROM courses WHERE course_id = $1", pqxx::params{course_id})};
            if (rows.empty())
            {
                return crow::response(404, json{{"error", "Course not found"}}.dump());
            }
            json course;
            course["course_id"]      = rows[0]["course_id"].as<std::string>();
            course["dept_code"]      = rows[0]["dept_code"].as<std::string>();
            course["course_name"]    = rows[0]["course_name"].as<std::string>();
            course["description"]    = rows[0]["description"].is_null() ? "" : rows[0]["description"].as<std::string>();
            course["capacity"]       = rows[0]["capacity"].as<int>();
            course["enrolled_count"] = rows[0]["enrolled_count"].as<int>();
            course["semester"]       = rows[0]["semester"].as<std::string>();
            course["day_of_week"]    = rows[0]["day_of_week"].as<std::string>();
            course["start_time"]     = rows[0]["start_time"].as<std::string>();
            course["end_time"]       = rows[0]["end_time"].as<std::string>();
            course["room"]           = rows[0]["room"].is_null() ? "" : rows[0]["room"].as<std::string>();
            course["is_active"]      = rows[0]["is_active"].as<bool>();
            return crow::response(200, json{course}.dump());
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
        catch (pqxx::unique_violation const &e)
        {
            return crow::response(409, json{{"error", "Course already registered"}}.dump());
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

    crow::response drop_course(pqxx::connection& cx, const crow::request& req, std::string course_id)
    {
        auto student_id = get_student_id(req);
        pqxx::work tx{cx};
        try
        {
            tx.exec("DELETE FROM registrations WHERE student_id = $1 AND course_id = $2", pqxx::params{student_id, course_id});
            tx.commit();
            return crow::response(200, json{{"success", true}}.dump());
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

    crow::response get_registered_courses(pqxx::connection& cx, const crow::request& req)
    {
        auto student_id = get_student_id(req);
        pqxx::work tx{cx};
        std::vector<json> courses;
        try
        {
            pqxx::result rows{tx.exec(
                "SELECT c.*, r.registered_at, r.status "
                "FROM registrations r "
                "JOIN courses c ON r.course_id = c.course_id "
                "WHERE r.student_id = $1",
                pqxx::params{student_id}
            )};
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
                course["registered_at"]  = row["registered_at"].as<std::string>();
                course["status"]         = row["status"].as<std::string>();
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
}
