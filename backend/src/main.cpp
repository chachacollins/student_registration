#include "routes/login.hpp"
#include "routes/registration.hpp"
#include "utils/utils.hpp"

struct JWTMiddleWare: crow::ILocalMiddleware
{
     struct context{};
     void before_handle(crow::request& req, crow::response& res, context&)
     {
         std::string myauth = req.get_header_value("Authorization");
         if (myauth.size() < 7)
         {
                 res.code = 400;
                 res.write("Bad Request: malformed authorization header");
                 res.end();
                 return;
         }
         std::string token  = myauth.substr(7);
         try
         {
             auto decoded = jwt::decode(token);
             utils::jwt_verify(decoded);
             auto student_id = decoded.get_payload_claim("student_id").as_string();
             if (student_id.empty())
             {
                 res.code = 400;
                 res.write("Bad Request: malformed jwt");
                 res.end();
             }
         }
         catch (const jwt::error::token_verification_exception& e) 
         {
             res.code = 401;
             res.write("Unauthorized: " + std::string(e.what()));
             res.end();
         }
         catch (const std::exception& e) 
         {
             res.code = 400;
             res.write("Bad Request: " + std::string(e.what()));
             res.end();
         }
     }
     void after_handle(crow::request&, crow::response&, context&){}
};

int main()
{
    crow::App<JWTMiddleWare> app;
    //TODO: put this in a dotenv
    pqxx::connection cx{"postgresql://srg_user:student_reg@localhost/student_reg"};

    CROW_ROUTE(app, "/")
    .CROW_MIDDLEWARES(app, JWTMiddleWare)
    ([](){
        return "Hello world";
    });

    CROW_ROUTE(app, "/register")
    .methods("POST"_method)
    ([&](const crow::request& req) {
        return routes::register_user(cx, req);
    });

    CROW_ROUTE(app, "/login")
    .methods("POST"_method)
    ([&](const crow::request& req) {
        return routes::login(cx, req);
    });

    app.port(1234).multithreaded().run();
}
