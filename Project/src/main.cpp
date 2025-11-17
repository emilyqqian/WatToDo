#include <crow.h>
#include <crow/middlewares/cors.h>
#include <iostream>
#include "database.h"

int main() {
    std::cout << "Compiled with:" << std::endl;
    std::cout << "MYSQL_USER: " << MYSQL_USER << std::endl;
    std::cout << "MYSQL_DB_NAME: " << MYSQL_DB_NAME << std::endl;
    initDatabase();
    
    // Use CORS middleware
    crow::App<crow::CORSHandler> app;

    // Configure CORS for React development server
    auto& cors = app.get_middleware<crow::CORSHandler>();
    
    cors
      .global()
      .origin("*")//.origin("http://localhost:5173")  // React dev server
      .methods("GET"_method, "POST"_method, "PUT"_method, "DELETE"_method, "OPTIONS"_method)
      .headers("Content-Type", "Authorization");

    // Complete Update user info endpoint with proper error handling
    CROW_ROUTE(app, "/users/<int>").methods("PUT"_method)
        ([](const crow::request& req, int user_id){
         try {
             auto json = crow::json::load(req.body);
             if (!json) return crow::response(400, "Invalid JSON");
             
             User user;
             user.userid = user_id;
             user.username = json["username"].s();
             user.password = json["password"].i();
             user.points = json["xp_points"].i();

             DatabaseResult result = updateUserInfo(user);

             switch(result) {
                 case SUCCESS:
                     return crow::response(200, "User updated successfully");
                 case DUPLICATE_NAME:
                     return crow::response(409, "Username already exists");
                 case DOES_NOT_EXIST:
                     return crow::response(404, "User not found");
                 case NAME_OVERFLOW:
                     return crow::response(400, "Username too long");
                 default:
                     return crow::response(500, "Update failed");
             }
         } catch (const std::exception& e) {
             return crow::response(500, "Server error");
         }
     });

    // Enhanced Register with proper error handling
    CROW_ROUTE(app, "/register").methods("POST"_method)
    ([](const crow::request& req){
        try {
            auto json = crow::json::load(req.body);
            if (!json) return crow::response(400, "Invalid JSON");
            
            std::string username = json["username"].s();
            unsigned int password = json["password"].i();
            
            DatabaseResult result = registerUser(username, password);
            
            switch(result) {
                case SUCCESS:
                    return crow::response(201, "User registered successfully");
                case DUPLICATE_NAME:
                    return crow::response(409, "Username already exists");
                case NAME_OVERFLOW:
                    return crow::response(400, "Username too long");
                default:
                    return crow::response(500, "Registration failed");
            }
        } catch (const std::exception& e) {
            return crow::response(500, "Server error");
        }
    });

    // Get user by username with proper error handling
    CROW_ROUTE(app, "/users/<string>")
        ([](const std::string& username){
         try {
             User user;
             DatabaseResult result = getUser(username, user);

             if (result == SUCCESS) {
                 crow::json::wvalue response;
                 response["userId"] = user.userid;
                 response["username"] = user.username;
                 response["password"] = user.password;
                 response["xp"] = user.points;
                 return crow::response(200, response);
             } else {
                 return crow::response(404, "User not found");
             }
         } catch (const std::exception& e) {
             return crow::response(500, "Server error");
         }
     });

    // Get user by ID with proper error handling
    CROW_ROUTE(app, "/users/id/<int>")
        ([](int user_id){
         try {
             User user;
             DatabaseResult result = getUser(user_id, user);

             if (result == SUCCESS) {
                 crow::json::wvalue response;
                 response["user_id"] = user.userid;
                 response["username"] = user.username;
                 response["xp_points"] = user.points;
                 return crow::response(200, response);
             } else {
                 return crow::response(404, "User not found");
             }
         } catch (const std::exception& e) {
             return crow::response(500, "Server error");
         }
     });

    // Leaderboard endpoint
    CROW_ROUTE(app, "/leaderboard")
        ([](){
         try {
             auto leaderboard = getLeaderboard();

             crow::json::wvalue response;
             int i = 0;
             for (const auto& [username, xp] : leaderboard) {
                 response["leaderboard"][i]["username"] = username;
                 response["leaderboard"][i]["xp"] = xp;
                 i++;
             }
             return crow::response(200, response);
         } catch (const std::exception& e) {
             return crow::response(500, "Server error");
         }
     });

    // Test endpoint
    CROW_ROUTE(app, "/test")([](){
        return "Crow server is running with complete user management!";
    });

    std::cout << "Starting WatToDo backend server on port 18080..." << std::endl;
    std::cout << "CORS enabled for React frontend (localhost:3000)" << std::endl;
    app.port(18080).run();
    
    closeDatabaseConnection();
    return 0;
}