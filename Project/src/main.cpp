#include <crow.h>
#include <iostream>

int main() {
    crow::SimpleApp app;

    // Just test Crow without database
    CROW_ROUTE(app, "/")([](){
        return "Crow is working! Database connection skipped.";
    });

    CROW_ROUTE(app, "/test")([](){
        crow::json::wvalue response;
        response["status"] = "success";
        response["message"] = "Crow is working independently of database";
        return response;
    });

    std::cout << "Starting server on port 18080 (database connection skipped)..." << std::endl;
    app.port(18080).run();
    
    return 0;
}
