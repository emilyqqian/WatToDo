#include <crow.h>
#include <crow/middlewares/cors.h>
#include <iostream>
#include "database.h"


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

      CROW_ROUTE(app, "/login").methods("POST"_method)
      ([](const crow::request& req){
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

                    //if (user.password == password){
                if (true){
                    crow::json::wvalue response;
                    response["userId"] = user.userid;
                    response["username"] = user.username;
                    response["password"] = user.password;
                    response["xp_points"] = user.points;
                    response["xp"] = user.points;
                    return crow::response(200, response);
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
            unsigned int owner_id = 1; // TODO: Replace with actual user ID from auth

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
      ([](int taskboard_id){
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
      ([](int user_id){
        try {
            std::vector<TaskBoard> taskboards;
            DatabaseResult result = getTaskBoardByUser(user_id, taskboards);
            
            if (result == SUCCESS) {
                crow::json::wvalue response;
                int i = 0;
                for (const auto& taskboard : taskboards) {
                    // DO NOT CHANGE THIS!!!!! @Yash
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
    // Test endpoint
      CROW_ROUTE(app, "/test")([](){
        return "Crow server is running with complete user management!";
    });

    CROW_ROUTE(app, "/removeUserFromBoard/<int>/<int>/<int>").methods("DELETE"_method)
      ([](const crow::request& req, int user, int taskboard_id, int performer){
        try {

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