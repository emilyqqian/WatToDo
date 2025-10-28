#include <string>
#include <vector>
#include <chrono>
#include <ctime>

#ifndef _DATABASE_H
#define _DATABASE_H

class User{
    public:
    unsigned int userid;
    std::string username;
    unsigned int password;

    User(std::string username, unsigned int password);
};

class Task{
    public:
    std::vector<User *> users;
    User *primaryUser;
    std::string title;
    std::string type;
    unsigned int taskID;
    bool pinned = false;
    time_t startDate;
    time_t duedate;
    time_t finishedOn;

    Task(User *primaryUser, std::string title, std::string type, time_t startDate, time_t duedate);
};

// return 1 if successful
// return 0 if user already exits
int addUser(User user);

// return 1 if successful
// return 0 if user does not exist
int updateUser(User user);

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
std::vector<Task> getTaskByUser(User user, bool showFinishedTasks);

#endif