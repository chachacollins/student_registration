#pragma once

namespace routes
{
    crow::response get_courses(pqxx::connection& cx, const crow::request& req);
    crow::response register_course(pqxx::connection& cx, const crow::request& req, std::string course_id);
    crow::response drop_course(pqxx::connection& cx, const crow::request& req, std::string course_id);
}
