#include <crow.h>
#include <crow/middlewares/cors.h>
#include "crow/middlewares/cookie_parser.h"
#include <iostream>
#include "database.h"
#include <cstdlib>
#include <unordered_map>

std::unordered_map<std::string, int> sessions;

int auth(const crow::request& req, int userId){
    std::string token = req.get_header_value("Authorization");
    if (token.length() < 8) {
        std::cerr << "Auth Token Not Found In Header" << std::endl;
        return 401;
    }
    token = token.substr(7);

    if (sessions.find(token) == sessions.end()){
        std::cerr << "Non-Existing Auth Token" << std::endl;
        return 401;
    }
    if (sessions[token] != userId) {
        std::cerr << "Incorrect Auth Token" << std::endl;
        return 403;
    }

    std::cout << "Auth Successful\n";

    return 0;
}

crow::json::wvalue getTaskBoardJSON(TaskBoard taskboard){
    crow::json::wvalue response;
    response["taskboard_id"] = taskboard.taskboard_id;
    response["name"] = taskboard.name;
                
    // Add users
    int user_index = 0;
    for (const auto& user : taskboard.users) {
        response["users"][user_index]["userId"] = user.userid;
        response["users"][user_index]["username"] = user.username;
        response["users"][user_index]["isAdmin"] = (taskboard.admins.find(user) != taskboard.admins.end());
        user_index++;
    }
                
    // Add tasks
    int task_index = 0;
    for (const auto& task : taskboard.tasklist) {
        response["tasks"][task_index]["task_id"] = task.taskID;
        response["tasks"][task_index]["title"] = task.title;
        response["tasks"][task_index]["type"] = task.type;
        response["tasks"][task_index]["start_date"] = task.startDate.toString();
        response["tasks"][task_index]["due_date"] = task.duedate.toString();
        response["tasks"][task_index]["pinned"] = task.pinned;
        response["tasks"][task_index]["finished"] = task.finished;
        if (task.finished) response["tasks"][task_index]["finished_date"] = task.finishedOn.toString();
                    
        if (task.assigned) {
            response["tasks"][task_index]["assignedUser"]["userId"] = task.assignedUser.userid;
            response["tasks"][task_index]["assignedUser"]["username"] = task.assignedUser.username;
        } else {
            response["tasks"][task_index]["assignedUser"] = nullptr;
        }
                    
        task_index++;
    }
    return response;
}

int main() {
    srand(time(0));
    std::cout << "Compiled with:" << std::endl;
    std::cout << "MYSQL_USER: " << MYSQL_USER << std::endl;
    std::cout << "MYSQL_DB_NAME: " << MYSQL_DB_NAME << std::endl;
    initDatabase();
    
    // Use CORS middleware
    crow::App<crow::CORSHandler, crow::CookieParser> app;

    // Configure CORS for React development server
    auto& cors = app.get_middleware<crow::CORSHandler>();

    cors
    .global()
      .origin("http://localhost:5173")  // React dev server
      .methods("GET"_method, "POST"_method, "PUT"_method, "DELETE"_method, "OPTIONS"_method)
      .headers("Content-Type", "Authorization")
      .allow_credentials();

    // Complete Update user info endpoint with proper error handling
      CROW_ROUTE(app, "/updateuser/<int>").methods("PUT"_method)
      ([](const crow::request& req, int user_id){
       try {
           auto json = crow::json::load(req.body);
           if (!json) return crow::response(400, "Invalid JSON");

           User user;
           user.userid = user_id;
           user.username = json["username"].s();
             user.password = json["password"].i();//this
             user.points = json["xp_points"].i();

             std::cout << "Request: " << user.username << " " << user.password << "\n";

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
            std::cout << "Error Code: " << result << "\n";
            return crow::response(500, "Update failed");
        }
    } catch (const std::exception& e) {
        std::cout<< "exception: \n" << e.what() << std::endl; 
        return crow::response(500, "Server error");
    }
});
    CROW_ROUTE(app, "/logout/<int>").methods("POST"_method)
    ([](const crow::request& req, int userId){
        std::string token = req.get_header_value("Authorization");
        if (token.length() < 8) {
            std::cerr << "Auth Token Not Found In Header" << std::endl;
            return crow::response(401, "Auth Token Not Found In Header");
        }
        token = token.substr(7);

        if (sessions.find(token) == sessions.end()){
            std::cerr << "Non-Existing Auth Token" << std::endl;
            return crow::response(401, "Auth Token Does Not Exist");
        }
        if (sessions[token] != userId) {
            std::cerr << "Incorrect Auth Token" << std::endl;
            return crow::response(403, "Incorrect Auth Token");
        }
        std::cout << "Logout Successful\n";
        sessions.erase(token);
        return crow::response(200, "Logout Successful");
    });

      CROW_ROUTE(app, "/auto-login").methods("POST"_method)
      ([&app](const crow::request& req){
        try{
            std::cout << "attempting to auto-login\n";

            auto& ctx = app.get_context<crow::CookieParser>(req);

            std::string cookieHeader = req.get_header_value("Cookie");
            std::string token;

            auto pos = cookieHeader.find("session_id=");
            if (pos != std::string::npos) {
                pos += strlen("session_id=");
                auto end = cookieHeader.find(';', pos);
                token = cookieHeader.substr(pos, end - pos);
            }

            // Check if a cookie exists
            if (token.empty()) {
                std::cout << "session id does not exist in cookie\n";
                return crow::response(404, "Session Id Not Found");
            }
            
            if (sessions.find(token) == sessions.end()){
                std::cerr << "Invalid" << std::endl;
                return crow::response(400, "Invalid Token");
            }

            User user;
            DatabaseResult result = getUser(sessions[token], user);
            
            crow::json::wvalue response;
            response["auth"] = token;
            response["userId"] = user.userid;
            response["username"] = user.username;
            response["password"] = user.password;
            response["xp_points"] = user.points;
            response["xp"] = user.points;
            std::cout << "Login Successful Using Cookie\n";

            crow::response res = crow::response(200, response);
            res.add_header("Set-Cookie", "session_id=" + token + "; Path=/; SameSite=None; HttpOnly");
            return res;
        }catch (const std::exception& e) {
            return crow::response(500, "Server error");
        }
      });

      CROW_ROUTE(app, "/login").methods("POST"_method)
      ([&app](const crow::request& req){
        try {
            auto json = crow::json::load(req.body);
            if (!json) return crow::response(400, "Invalid JSON");
            
            std::string username = json["username"].s();
            unsigned int password = json["password"].i();
            User user;
            
            DatabaseResult result = getUser(username, user);
            
            switch(result) {
            case SUCCESS:
                    // who cares about password?
                    // I cares about password

                if (user.password == password){
                //if (true){
                    std::string sessionId = std::to_string(rand());
                    sessions[sessionId] = user.userid;
                    std::cout << "Created session " << sessionId << " for user " << user.userid << std::endl;

                    //crow::CookieParser::Cookie myCookie("session_id", sessionId);
                    //myCookie.max_age(3600 * 24 * 3); // Expires in 3 days
                    //myCookie.path("/");
                    //myCookie.secure(); // Only send over HTTPS
                    //app.get_context<crow::CookieParser>(req).set_cookie(myCookie);
                    //auto& ctx = app.get_context<crow::CookieParser>(req);
                    //ctx.set_cookie("session_id", sessionId)
                    //    .path("/")
                    //    .max_age(3600); // Expires in 1 hour
                    

                    crow::json::wvalue response;
                    response["auth"] = sessionId;
                    response["userId"] = user.userid;
                    response["username"] = user.username;
                    response["password"] = user.password;
                    response["xp_points"] = user.points;
                    response["xp"] = user.points;
                    crow::response res = crow::response(200, response);
                    res.add_header("Set-Cookie", "session_id=" + sessionId + "; Path=/; SameSite=None; HttpOnly");
                    return res;
                }
                else return crow::response(409, "Wrong Username or Password");
            case DOES_NOT_EXIST:
                return crow::response(409, "Wrong Username or Password");
            default:
                return crow::response(500, "Registration failed");
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
                 response["xp_points"] = user.points;
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
                 response["leaderboard"][i]["xp_points"] = xp;
                 i++;
             }
             return crow::response(200, response);
         } catch (const std::exception& e) {
             return crow::response(500, "Server error");
         }
     });

    // Add Task to Taskboard
      CROW_ROUTE(app, "/addTask/<int>/<int>").methods("POST"_method)
      ([](const crow::request& req, int board_id, int performed_by){
        try {

            int authed = auth(req, performed_by);
            if (authed){
                crow::json::wvalue error;
                error["error"] = "Authentication Failed";
                return crow::response(authed, error);
            }

            auto json = crow::json::load(req.body);
            crow::json::wvalue response;

            if (!json) {
                crow::json::wvalue error;
                error["error"] = "Invalid JSON";
                return crow::response(400, error);
            }
            
            Task task;
            task.boardID = board_id;
            task.title = json["title"].s();
            task.type = json["type"].s();
            task.startDate = date(json["start_date"].s());
            task.duedate = date(json["due_date"].s());
            
            // Optional fields with defaults
            if (json.has("assigned_user_id")) {
                task.assigned = true;
                User assignedUser;
                DatabaseResult userResult = getUser(json["assigned_user_id"].i(), assignedUser);
                if (userResult != SUCCESS) {
                    crow::json::wvalue error;
                    error["error"] = "Assigned user not found";
                    return crow::response(404, error);
                }
                task.assignedUser = assignedUser;
            } else {
                task.assigned = false;
            }
            
            task.pinned = json.has("pinned") ? json["pinned"].b() : false;
            task.finished = false;

            // For now, use a default user ID - you'll want to get this from authentication
            // fuck you
            //unsigned int performed_by = 1; // TODO: Replace with actual user ID from auth

            DatabaseResult result = addTask(board_id, task, performed_by);
            
            switch(result) {
            case SUCCESS:
                response["message"] = "Task created successfully";
                response["task_id"] = task.taskID;
                return crow::response(201, response);
            case USER_ACCESS_DENINED:
                response["error"] = "Access denied to taskboard";
                return crow::response(403, response);
            case NAME_OVERFLOW:
                response["error"] = "Task title or type too long";
                return crow::response(400, response);
            case TIME_CONFLICT:
                response["error"] = "Invalid dates - due date must be after start date and in the future";
                return crow::response(400, response);
            case ALREADY_EXIST:
                response["error"] = "Task with same title and type already exists";
                return crow::response(400, response);
            default:
                response["error"] = "Failed to create task";
                return crow::response(500, response);
            }
        } catch (const std::exception& e) {
            crow::json::wvalue error;
            error["error"] = "Server error";
            return crow::response(500, error);
        }
    });

// Get Task by ID
      CROW_ROUTE(app, "/tasks/<int>")
      ([](int task_id){
        try {
            Task task;
            DatabaseResult result = getTask(task_id, task);

            if (result == SUCCESS) {
                crow::json::wvalue response;
                response["task_id"] = task.taskID;
                response["board_id"] = task.boardID;
                response["title"] = task.title;
                response["type"] = task.type;
                response["start_date"] = task.startDate.toString();
                response["due_date"] = task.duedate.toString();
                response["pinned"] = task.pinned;
                response["finished"] = task.finished;
                
                if (task.assigned) {
                    response["assigned_user"]["user_id"] = task.assignedUser.userid;
                    response["assigned_user"]["username"] = task.assignedUser.username;
                } else {
                    response["assigned_user"] = nullptr;
                }
                
                if (task.finished) {
                    response["finished_date"] = task.finishedOn.toString();
                } else {
                    response["finished_date"] = nullptr;
                }
                
                return crow::response(200, response);
            } else {
                crow::json::wvalue error;
                error["error"] = "Task not found";
                return crow::response(404, error);
            }
        } catch (const std::exception& e) {
            crow::json::wvalue error;
            error["error"] = "Server error";
            return crow::response(500, error);
        }
    });

// Update Task
      CROW_ROUTE(app, "/updateTask/<int>/<int>").methods("PUT"_method)
      ([](const crow::request& req, int task_id, int performed_by){
        try {

            int authed = auth(req, performed_by);
            if (authed){
                crow::json::wvalue error;
                error["error"] = "Authentication Failed";
                return crow::response(authed, error);
            }

            auto json = crow::json::load(req.body);
            if (!json) {
                crow::json::wvalue error;
                error["error"] = "Invalid JSON";
                return crow::response(400, error);
            }
            
            Task existingTask;
            DatabaseResult getResult = getTask(task_id, existingTask);
            if (getResult != SUCCESS) {
                crow::json::wvalue error;
                error["error"] = "Task not found";
                return crow::response(404, error);
            }
            
            // Update fields if provided
            if (json.has("title")) existingTask.title = json["title"].s();
            if (json.has("type")) existingTask.type = json["type"].s();
            if (json.has("start_date")) existingTask.startDate = date(json["start_date"].s());
            if (json.has("due_date")) existingTask.duedate = date(json["due_date"].s());
            if (json.has("pinned")) existingTask.pinned = json["pinned"].b();
            
            // Handle assignment
            if (json.has("assigned_user_id")) {
                if (json["assigned_user_id"] == 0) {
                    existingTask.assigned = false;
                } else {
                    existingTask.assigned = true;
                    User assignedUser;
                    DatabaseResult userResult = getUser(json["assigned_user_id"].i(), assignedUser);
                    if (userResult != SUCCESS) {
                        crow::json::wvalue error;
                        error["error"] = "Assigned user not found";
                        return crow::response(404, error);
                    }
                    existingTask.assignedUser = assignedUser;
                }
            }
            
            // Handle completion
            if (json.has("finished") && !existingTask.finished) {
                existingTask.finished = json["finished"].b();
                if (existingTask.finished) {
                    existingTask.finishedOn = date(); // current date
                }
            }

            // For now, use a default user ID
            // Fuck you
            //unsigned int performed_by = 1; // TODO: Replace with actual user ID from auth

            DatabaseResult result = updateTask(existingTask, performed_by);
            
            crow::json::wvalue response;
            switch(result) {
            case SUCCESS:
                response["message"] = "Task updated successfully";
                return crow::response(200, response);
            case USER_ACCESS_DENINED:
                response["error"] = "Access denied to taskboard";
                return crow::response(403, response);
            case NAME_OVERFLOW:
                response["error"] = "Task title or type too long";
                return crow::response(400, response);
            case TIME_CONFLICT:
                response["error"] = "Invalid dates";
                return crow::response(400, response);
            case DOES_NOT_EXIST:
                response["error"] = "Task not found";
                return crow::response(404, response);
            default:
                response["error"] = "Failed to update task";
                return crow::response(500, response);
            }
        } catch (const std::exception& e) {
            crow::json::wvalue error;
            error["error"] = "Server error";
            return crow::response(500, error);
        }
    });

    // Delete Task
      CROW_ROUTE(app, "/tasks/<int>").methods("DELETE"_method)
      ([](const crow::request& req, int task_id){
        try {
                // For now, use a default user ID
                //unsigned int performed_by = 1; // TODO: Replace with actual user ID from auth

                unsigned int performed_by = crow::json::load(req.body)["operator"].i();

            int authed = auth(req, performed_by);
            if (authed){
                crow::json::wvalue error;
                error["error"] = "Authentication Failed";
                return crow::response(authed, error);
            }

                DatabaseResult result = deleteTask(task_id, performed_by);
                
                crow::json::wvalue response;
                switch(result) {
                case SUCCESS:
                    response["message"] = "Task deleted successfully";
                    return crow::response(200, response);
                case USER_ACCESS_DENINED:
                    response["error"] = "Access denied to taskboard";
                    return crow::response(403, response);
                case DOES_NOT_EXIST:
                    response["error"] = "Task not found";
                    return crow::response(404, response);
                default:
                    response["error"] = "Failed to delete task";
                    return crow::response(500, response);
                }
            } catch (const std::exception& e) {
                crow::json::wvalue error;
                error["error"] = "Server error";
                return crow::response(500, error);
            }
        });

    // Get Assigned Tasks for User
      CROW_ROUTE(app, "/users/<int>/assigned-tasks")
      ([](int user_id){
        try {
            std::vector<Task> tasks;
            DatabaseResult result = getAssignedTaskForUser(user_id, tasks);

            if (result == SUCCESS) {
                crow::json::wvalue response;
                int i = 0;
                for (const auto& task : tasks) {
                    response["tasks"][i]["task_id"] = task.taskID;
                    response["tasks"][i]["board_id"] = task.boardID;
                    response["tasks"][i]["title"] = task.title;
                    response["tasks"][i]["type"] = task.type;
                    response["tasks"][i]["start_date"] = task.startDate.toString();
                    response["tasks"][i]["due_date"] = task.duedate.toString();
                    response["tasks"][i]["pinned"] = task.pinned;
                    response["tasks"][i]["finished"] = task.finished;
                    i++;
                }
                return crow::response(200, response);
            } else {
                crow::json::wvalue error;
                error["error"] = "User not found";
                return crow::response(404, error);
            }
        } catch (const std::exception& e) {
            crow::json::wvalue error;
            error["error"] = "Server error";
            return crow::response(500, error);
        }
    });
    // Create Taskboard
      CROW_ROUTE(app, "/taskboards").methods("POST"_method)
      ([](const crow::request& req){
        try {
            auto json = crow::json::load(req.body);
            if (!json) {
                crow::json::wvalue error;
                error["error"] = "Invalid JSON";
                return crow::response(400, error); // invalid json
            }
            
            std::string name = json["name"].s();
            
            // For now, use a default user ID - you'll want to get this from authentication
            unsigned int owner_id = json["owner"].i();//1; // TODO: Replace with actual user ID from auth

            int authed = auth(req, owner_id);
            if (authed){
                crow::json::wvalue error;
                error["error"] = "Authentication Failed";
                return crow::response(authed, error);
            }

            TaskBoard createdTaskboard;
            DatabaseResult result = createTaskBoard(owner_id, name, createdTaskboard);
            
            crow::json::wvalue response;
            switch(result) {
            case SUCCESS:
                response["message"] = "Taskboard created successfully";
                response["taskboard_id"] = createdTaskboard.taskboard_id;
                response["name"] = createdTaskboard.name;
                return crow::response(201, response);
            case NAME_OVERFLOW:
                response["error"] = "Taskboard name too long";
                return crow::response(400, response);
            case DOES_NOT_EXIST:
                response["error"] = "Owner user not found";
                return crow::response(404, response);
            default:
                response["error"] = "Failed to create taskboard";
                return crow::response(500, response);
            }
        } catch (const std::exception& e) {
            crow::json::wvalue error;
            error["error"] = "Server error";
            return crow::response(500, error);
        }
    });

// Get Taskboard by ID
      CROW_ROUTE(app, "/taskboards/<int>")
      ([](const crow::request& req, int taskboard_id){
        try {
            TaskBoard taskboard;
            DatabaseResult result = getTaskBoardByID(taskboard_id, taskboard);

            if (result == SUCCESS) {
                return crow::response(200, getTaskBoardJSON(taskboard));
            } else {
                crow::json::wvalue error;
                error["error"] = "Taskboard not found"; // user does not have a taskboard 
                return crow::response(404, error);
            }
        } catch (const std::exception& e) {
            crow::json::wvalue error;
            error["error"] = "Server error";
            return crow::response(500, error);
        }
    });

// Get Taskboards for User
      CROW_ROUTE(app, "/users/<int>/taskboards")
      ([](const crow::request& req, int user_id){
        try {
            int authed = auth(req, user_id);
            if (authed){
                crow::json::wvalue error;
                error["error"] = "Authentication Failed";
                return crow::response(authed, error);
            }

            std::vector<TaskBoard> taskboards;
            DatabaseResult result = getTaskBoardByUser(user_id, taskboards);
            
            if (result == SUCCESS) {
                crow::json::wvalue response;
                int i = 0;
                for (const auto& taskboard : taskboards) {
                    // DO NOT CHANGE THIS!!!!! @Yash
                    std::cout << "Recovered " << taskboard.tasklist.size() << " Tasks for " << taskboard.name << '\n';
                    response["taskboards"][i] = getTaskBoardJSON(taskboard);
                    i++;
                }
                return crow::response(200, response);
            } else {
                crow::json::wvalue error;
                error["error"] = "User not found";
                return crow::response(404, error);
            }
        } catch (const std::exception& e) {
            crow::json::wvalue error;
            error["error"] = "Server error";
            return crow::response(500, error);
        }
    });

// Rename Taskboard
      CROW_ROUTE(app, "/taskboards/<int>/rename/<int>").methods("PUT"_method)
      ([](const crow::request& req, int taskboard_id, int performed_by){
        try {
            int authed = auth(req, performed_by);
            if (authed){
                crow::json::wvalue error;
                error["error"] = "Authentication Failed";
                return crow::response(authed, error);
            }

            auto json = crow::json::load(req.body);
            if (!json) {
                crow::json::wvalue error;
                error["error"] = "Invalid JSON";
                return crow::response(400, error);
            }

            std::string new_name = json["name"].s();
            
            // For now, use a default user ID
            //unsigned int performed_by = 1; // TODO: Replace with actual user ID from auth

            DatabaseResult result = renameTaskBoard(taskboard_id, new_name, performed_by);
            
            crow::json::wvalue response;
            switch(result) {
            case SUCCESS:
                response["message"] = "Taskboard renamed successfully";
                return crow::response(200, response);
            case USER_ACCESS_DENINED:
                response["error"] = "Access denied to taskboard";
                return crow::response(403, response);
            case NAME_OVERFLOW:
                response["error"] = "Taskboard name too long";
                return crow::response(400, response);
            case DOES_NOT_EXIST:
                response["error"] = "Taskboard not found";
                return crow::response(404, response);
            default:
                response["error"] = "Failed to rename taskboard";
                return crow::response(500, response);
            }
        } catch (const std::exception& e) {
            crow::json::wvalue error;
            error["error"] = "Server error";
            std::cout << "exception: " << e.what() << "\n";
            return crow::response(500, error);
        }
    });

// Delete Taskboard
      CROW_ROUTE(app, "/taskboards/<int>/<int>").methods("DELETE"_method)
      ([](const crow::request& req, int taskboard_id, int performed_by){
        try {
            int authed = auth(req, performed_by);
            if (authed){
                crow::json::wvalue error;
                error["error"] = "Authentication Failed";
                return crow::response(authed, error);
            }

            DatabaseResult result = deleteTaskBoard(taskboard_id, performed_by);
            
            crow::json::wvalue response;
            switch(result) {
            case SUCCESS:
                response["message"] = "Taskboard deleted successfully";
                return crow::response(200, response);
            case USER_ACCESS_DENINED:
                response["error"] = "Access denied to taskboard";
                return crow::response(403, response);
            case DOES_NOT_EXIST:
                response["error"] = "Taskboard not found";
                return crow::response(404, response);
            default:
                response["error"] = "Failed to delete taskboard";
                return crow::response(500, response);
            }
        } catch (const std::exception& e) {
            crow::json::wvalue error;
            error["error"] = "Server error";
            return crow::response(500, error);
        }
    });

    // Update User Status in Taskboard (Make Admin/Remove Admin)
    CROW_ROUTE(app, "/taskboards/<int>/users/<int>/status").methods("PUT"_method)
    ([](const crow::request& req, int taskboard_id, int user_id){
        try {
            int authed = auth(req, user_id);
            if (authed){
                crow::json::wvalue error;
                error["error"] = "Authentication Failed";
                return crow::response(authed, error);
            }

            auto json = crow::json::load(req.body);
            if (!json) {
                crow::json::wvalue error;
                error["error"] = "Invalid JSON";
                return crow::response(400, error);
            }
            
            bool isAdmin = json["is_admin"].b();
            unsigned int performed_by = json["user_id"].i();

            DatabaseResult result = updateUserStatus(user_id, taskboard_id, isAdmin, performed_by);
            
            crow::json::wvalue response;
            switch(result) {
                case SUCCESS:
                    response["message"] = "User status updated successfully";
                    return crow::response(200, response);
                case USER_ACCESS_DENINED:
                    response["error"] = "Access denied to taskboard";
                    return crow::response(403, response);
                case DOES_NOT_EXIST:
                    response["error"] = "User or taskboard not found";
                    return crow::response(404, response);
                case USER_CONFLICT:
                    response["error"] = "Cannot remove the last admin";
                    return crow::response(409, response);
                default:
                    response["error"] = "Failed to update user status";
                    return crow::response(500, response);
            }
        } catch (const std::exception& e) {
            crow::json::wvalue error;
            error["error"] = "Server error";
            return crow::response(500, error);
        }
    });

    // Add User to Taskboard
    CROW_ROUTE(app, "/taskboards/<int>/users").methods("POST"_method)
    ([](const crow::request& req, int taskboard_id){
        try {
            auto json = crow::json::load(req.body);
            if (!json) {
                crow::json::wvalue error;
                error["error"] = "Invalid JSON";
                return crow::response(400, error);
            }
            
            unsigned int user_id = json["user_id"].i();
            unsigned int performed_by = json["host"].i();

            DatabaseResult result = addUserToTaskboard(user_id, taskboard_id, performed_by);
            
            crow::json::wvalue response;
            switch(result) {
                case SUCCESS:
                    response["message"] = "User added to taskboard successfully";
                    return crow::response(200, response);
                case USER_ACCESS_DENINED:
                    response["error"] = "Access denied or user not invited";
                    return crow::response(403, response);
                case ALREADY_EXIST:
                    response["error"] = "User already in taskboard";
                    return crow::response(409, response);
                case DOES_NOT_EXIST:
                    response["error"] = "User or taskboard not found";
                    return crow::response(404, response);
                default:
                    response["error"] = "Failed to add user to taskboard";
                    return crow::response(500, response);
            }
        } catch (const std::exception& e) {
            crow::json::wvalue error;
            error["error"] = "Server error";
            return crow::response(500, error);
        }
    });

    // reject Invitations
    CROW_ROUTE(app, "/users/<int>/reject/<int>")
    ([](const crow::request& req, int user_id, int board_id){
        std::cout << "rejecting invitation\n";

        try {
            DatabaseResult result = rejectInvitation(user_id, board_id);
            
            if (result == SUCCESS) {
                crow::json::wvalue response;
                response["message"] = "Success";
                return crow::response(200, response);
            } else {
                crow::json::wvalue error;
                error["error"] = "Failed to get invitations";
                return crow::response(500, error);
            }
        } catch (const std::exception& e) {
            crow::json::wvalue error;
            error["error"] = "Server error";
            return crow::response(500, error);
        }
    });

    // Invite User to Taskboard
    CROW_ROUTE(app, "/taskboards/<int>/invite").methods("POST"_method)
    ([](const crow::request& req, int taskboard_id){
        try {
            auto json = crow::json::load(req.body);
            if (!json) {
                crow::json::wvalue error;
                error["error"] = "Invalid JSON";
                return crow::response(400, error);
            }
            
            unsigned int to_user_id = json["to"].i();
            unsigned int from_user_id = json["from"].i();

            int authed = auth(req, from_user_id);
            if (authed){
                crow::json::wvalue error;
                error["error"] = "Authentication Failed";
                return crow::response(authed, error);
            }

            DatabaseResult result = inviteUser(from_user_id, to_user_id, taskboard_id);
            
            crow::json::wvalue response;
            switch(result) {
                case SUCCESS:
                    response["message"] = "Invitation sent successfully";
                    return crow::response(201, response);
                case USER_ACCESS_DENINED:
                    response["error"] = "Access denied to taskboard";
                    return crow::response(403, response);
                case ALREADY_EXIST:
                    response["error"] = "User already in taskboard or have already been invited by your";
                    return crow::response(409, response);
                case DOES_NOT_EXIST:
                    response["error"] = "User or taskboard not found";
                    return crow::response(404, response);
                default:
                    response["error"] = "Failed to send invitation";
                    return crow::response(500, response);
            }
        } catch (const std::exception& e) {
            crow::json::wvalue error;
            error["error"] = "Server error";
            return crow::response(500, error);
        }
    });

    // Get User's Invitations
    CROW_ROUTE(app, "/users/<int>/invitations")
    ([](const crow::request& req, int user_id){
        try {

            int authed = auth(req, user_id);
            if (authed){
                crow::json::wvalue error;
                error["error"] = "Authentication Failed";
                return crow::response(authed, error);
            }

            std::unordered_multimap<User, TaskBoard, UserHasher> invitations;
            DatabaseResult result = getAllInvitation(user_id, invitations);
            
            if (result == SUCCESS) {
                crow::json::wvalue response;
                int i = 0;
                for (const auto& [inviter, taskboard] : invitations) {
                    response["invitations"][i]["inviter"]["user_id"] = inviter.userid;
                    response["invitations"][i]["inviter"]["username"] = inviter.username;
                    response["invitations"][i]["taskboard"]["taskboard_id"] = taskboard.taskboard_id;
                    response["invitations"][i]["taskboard"]["name"] = taskboard.name;
                    i++;
                }
                return crow::response(200, response);
            } else {
                crow::json::wvalue error;
                error["error"] = "Failed to get invitations";
                return crow::response(500, error);
            }
        } catch (const std::exception& e) {
            crow::json::wvalue error;
            error["error"] = "Server error";
            return crow::response(500, error);
        }
    });


    // Test endpoint
      CROW_ROUTE(app, "/test")([](){
        return "Crow server is running with complete user management!";
    });

    CROW_ROUTE(app, "/removeUserFromBoard/<int>/<int>/<int>").methods("DELETE"_method)
      ([](const crow::request& req, int user, int taskboard_id, int performer){
        try {

            int authed = auth(req, performer);
            if (authed){
                crow::json::wvalue error;
                error["error"] = "Authentication Failed";
                return crow::response(authed, error);
            }

            DatabaseResult res = kickOutUserFromTaskboard(user, taskboard_id, performer);
            
            crow::json::wvalue response;
            switch(res) {
            case SUCCESS:
                response["message"] = "deleted successfully";
                return crow::response(200, response);
            case USER_ACCESS_DENINED:
                response["error"] = "Access denied to taskboard";
                return crow::response(403, response);
            case DOES_NOT_EXIST:
                response["error"] = "User not found";
                return crow::response(404, response);
            case USER_CONFLICT:
                response["error"] = "User Conflict";
                return crow::response(409, response);
            default:
                response["error"] = "Server Error";
                return crow::response(500, response);
            }
        } catch (const std::exception& e) {
            crow::json::wvalue error;
            error["error"] = "Server error";
            return crow::response(500, error);
        }
      });

      std::cout << "Starting WatToDo backend server on port 18080..." << std::endl;
      std::cout << "CORS enabled for React frontend (localhost:3000)" << std::endl;
      app.port(18080).run();

      closeDatabaseConnection();
      return 0;
  }