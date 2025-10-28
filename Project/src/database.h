#include <string>
#include <vector>
#include <chrono>

#ifndef _DATABASE_H
#define _DATABASE_H

class User{
    public:
    unsigned int userid;
    std::string username;
    unsigned int password;
};

class Task{
    std::vector<User> users;
    User primaryUser;
    std::string title;
    std::string type;
    unsigned int taskID;
    bool pinned;
};

// return 1 if successful
// return 0 if date error
// return -1 if other error

// note: you do not need to assign taskID, I will cover that
int addTask(Task task);

// return 1 if successful
// return 0 if task does not exist
int deleteTask(Task task);

// return 1 if successful
// return 0 if date error
// return -1 if task does not exist
int updateTask(Task task);

// returns all tasks associated with that user
std::vector<Task> getTaskByUser(User user);

#endif