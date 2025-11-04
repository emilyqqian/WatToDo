#include <crow.h>
#include <iostream>
#include "database.h"


int main() {
    std::cout << "Compiled with:" << std::endl;
    std::cout << "MYSQL_USER: " << MYSQL_USER << std::endl;
    std::cout << "MYSQL_DB_NAME: " << MYSQL_DB_NAME << std::endl;
    initDatabase();
    crow::SimpleApp app;
    // Frontend sends: { "username": "john", "password": 123 }
    CROW_ROUTE(app, "/register").methods("POST"_method)
    ([](const crow::request& req){
        // 1. Extract JSON from HTTP request
        auto json = crow::json::load(req.body);
        std::string username = json["username"].s();
        unsigned int password = json["password"].i();
        
        // 2. Call existing database function
        DatabaseResult result = registerUser(username, password);
        
        // 3. Convert result to HTTP response
        return crow::response(201, "User registered");
        std::cout << "DEBUG: registerUser returned: " << result << std::endl;
    });

    CROW_ROUTE(app, "/users/<string>")
        ([](const std::string& username){

         User user;
         DatabaseResult result = getUser(username, user);

          crow::json::wvalue response;
          response["user_id"] = user.userid;
          response["username"] = user.username;
          return crow::response(200, response);
          std::cout << "DEBUG: getUser returned: " << result << std::endl;
     });

    // Just test Crow without database
    CROW_ROUTE(app, "/test")([](){
        crow::json::wvalue response;
        response["status"] = "success";
        response["message"] = "Crow is working independently of database";
        return response;
    });

    std::cout << "Starting server on port 18080 (database connection skipped)..." << std::endl;
    app.port(18080).run();
    closeDatabaseConnection();
    return 0;
}
