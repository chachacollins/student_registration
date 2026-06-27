#pragma once

namespace routes
{
    crow::response login(pqxx::connection& cx, const crow::request& req);
}
