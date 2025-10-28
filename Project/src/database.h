#include <string>
#include <set>
#include <chrono>
#include <ctime>
#include <unordered_map>

#ifndef _DATABASE_H
#define _DATABASE_H

class User{
    public:

    unsigned int userid;
    std::string username;// username has to be unique
    unsigned int password;// hash of password
	unsigned int points = 0;// points the user has earned

    // used internally
    User(unsigned int userid, std::string username, unsigned int password);
};

class Task{
    public:
    unsigned int taskID;
    std::string title;
    std::string type;
    bool pinned = false;
    time_t startDate;
    time_t duedate;
    time_t finishedOn;
    bool finished = false;

    // used to create a new task, only necessary informations are included
    Task(std::string title, std::string type, time_t startDate, time_t duedate);

    // used to recreate a task form database/frontend
    Task(std::string title, std::string type, unsigned int taskID, bool pinned, time_t startDate, time_t duedate, time_t finished, bool isFinished);
};

class TaskBoard{
    public:
    unsigned int taskboard_id;
    std::string name;
    std::set<Task> tasklist;
    std::set<User> users;
    std::set<User> admins;
};

enum DatabaseResult{
    SUCCESS, // everything goes OK
    DOES_NOT_EXIST, // something should exist but does not exist
    ALREADY_EXIST, // something should not exist but found exist
    TIME_CONFLICT, // the task's time is unresonable. This should be checked by the frontend, but again, we shouln't trust that the frontend always returns the correct data, so we check again here
    USER_CONFLICT, // when updating taskboard, thrown if a user is an admin but not a member
    SQL_ERROR, // there is an error when sending sql to database
    USER_ACCESS_DENINED, // when a user who is not admin try to modify the task board
    DUPLICATE_NAME, // when multiple users having the same username
    OTHER_ERROR // unknown error
};

//init function
void initDatabase();// needs to be called first

// helper functions
// Im trying to use id instead of the whole class as much as possible
// so the frontend don't need to give back the entire user/task/taskboard

// functions about user
DatabaseResult registerUser(std::string, unsigned int password);
DatabaseResult getUser(std::string username, User &returnedUser);
DatabaseResult updateUserInfo(User user);

// functions about user inviting other users to join taskboard

// invite a user to join the taskboard
DatabaseResult inviteUser(User fromUser, std::string toUserName, unsigned int taskBoard_id); 
// returns all invitations the user "whom" has received
// the key of the dictionary is "who invits you", and the value of the dictionary is "to what taskboard"
// there is no "accept invitation" thing, you just directly updtae the taskboard, thats enough
DatabaseResult getAllInvitation(unsigned int user_id, std::unordered_multimap<User, TaskBoard> &returnedInvitations);

// functions about task
DatabaseResult addTask(unsigned int taskboard_id, Task task);// note: you do not need to assign taskID and all other part, I will cover that
DatabaseResult deleteTask(Task task);
DatabaseResult updateTask(Task task);

// functions about taskBoard
DatabaseResult getTaskBoardByUser(unsigned int user_id, std::vector<TaskBoard> &returnedTaskList);
DatabaseResult createTaskBoard(User owner);
// Note: the updateTaskBoard function should only be called to update members and names. 
// If you want to update the task list, call addTask, updateTask, or deleteTask individually
// This is because mysql cannot store array, so updating tasks through updating taskBoard would require you to 
// iterate through the entire task table, check the difference, and update, which is extremely slow
DatabaseResult updateTaskBoard(TaskBoard taskboard);
DatabaseResult deleteTaskBoard(TaskBoard taskboard);

// functions about leaderboard
std::unordered_map<User, unsigned int> getLeaderboard();// I don't think theres a need to use DatabaseResult here, since this function will always succeed
// if you want to set xp, just call updateUserInfo
// if you want to get xp, just call getLeaderboard() and find the user. The cost are negligible
#endif