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

    // Add Task to Taskboard
      CROW_ROUTE(app, "/taskboards/<int>/tasks").methods("POST"_method)
      ([](const crow::request& req, int board_id){
        try {
            auto json = crow::json::load(req.body);
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
            unsigned int performed_by = 1; // TODO: Replace with actual user ID from auth

            DatabaseResult result = addTask(board_id, task, performed_by);
            
            crow::json::wvalue response;
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
                return crow::response(409, response);
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
      CROW_ROUTE(app, "/tasks/<int>").methods("PUT"_method)
      ([](const crow::request& req, int task_id){
        try {
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
                if (json["assigned_user_id"].isNull()) {
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
            if (json.has("finished")) {
                existingTask.finished = json["finished"].b();
                if (existingTask.finished) {
                    existingTask.finishedOn = date(); // current date
                }
            }

            // For now, use a default user ID
            unsigned int performed_by = 1; // TODO: Replace with actual user ID from auth

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
                unsigned int performed_by = 1; // TODO: Replace with actual user ID from auth

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
            crow::json::wvalueß error;
            error["error"] = "Server error";
            return crow::response(500, error);
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