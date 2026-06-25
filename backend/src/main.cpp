#include "routes/registration.hpp"

int main()
{
    crow::SimpleApp app;
    //TODO: put this in a dotenv
    pqxx::connection cx{"postgresql://srg_user:student_reg@localhost/student_reg"};

    CROW_ROUTE(app, "/")([](){
        return "Hello world";
    });

    CROW_ROUTE(app, "/register")
    .methods("POST"_method)
    ([&](const crow::request& req) {
        return routes::register_user(cx, req);
    });

    app.port(1234).multithreaded().run();
}
