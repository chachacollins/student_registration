#pragma once
namespace routes
{
    crow::response register_user(pqxx::connection& cx, const crow::request& req);
}
