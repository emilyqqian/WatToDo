#include <string>
#include <unordered_map>
#include "databaseClasses.h"

#ifndef _DATABASE_H
#define _DATABASE_H

enum DatabaseResult{
    SUCCESS, // everything goes OK
    DOES_NOT_EXIST, // something should exist but does not exist
    ALREADY_EXIST, // something should not exist but found exist
    TIME_CONFLICT, // the task's time is unresonable. This should be checked by the frontend, but again, we shouln't trust that the frontend always returns the correct data, so we check again here
    USER_CONFLICT, // when updating taskboard, thrown if a user is an admin but not a member
    SQL_ERROR, // there is an error when sending sql to database
    USER_ACCESS_DENINED, // when a user who is not admin try to modify the task board
    DUPLICATE_NAME, // when multiple users having the same username
    NAME_OVERFLOW, // when the length of a string exceeds the maximum length allowed
    OTHER_ERROR // unknown error
};

//init function
void initDatabase();// needs to be called first
void closeDatabaseConnection();// if you want to pass valgrind, call this to close the connection properly

// helper functions
// Im trying to use id instead of the whole class as much as possible
// so the frontend don't need to give back the entire user/task/taskboard

// functions about user
DatabaseResult registerUser(std::string username, unsigned int password);
DatabaseResult getUser(std::string username, User &returnedUser);
DatabaseResult getUser(unsigned int userid, User& returnedUser);
DatabaseResult updateUserInfo(User user);

// functions about user inviting other users to join taskboard

// invite a user to join the taskboard
DatabaseResult inviteUser(User fromUser, unsigned int toUser, unsigned int taskBoard_id); 
// returns all invitations the user "whom" has received
// the key of the dictionary is "who invits you", and the value of the dictionary is "to what taskboard"
// there is no "accept invitation" thing, you just directly updtae the taskboard, thats enough
DatabaseResult getAllInvitation(unsigned int user_id, std::unordered_multimap<User, TaskBoard> &returnedInvitations);

// functions about task
DatabaseResult addTask(unsigned int taskboard_id, Task &task);// note: you do not need to assign taskID. This will automatically set an id for you
DatabaseResult deleteTask(unsigned int task_id, unsigned int performed_by);
DatabaseResult updateTask(Task task, unsigned int performed_by);
DatabaseResult getTask(unsigned int task_id, Task &returnedTask);
DatabaseResult getAssignedTaskForUser(unsigned int user_id, std::vector<Task> &returnedTaskList);

// functions about taskBoard
DatabaseResult getTaskBoardByUser(unsigned int user_id, std::vector<TaskBoard> &returnedTaskList);
DatabaseResult createTaskBoard(User owner);
DatabaseResult updateUserStatus(unsigned int user_id, unsigned int taskboard_id, bool isAdmin);
DatabaseResult addUserToTaskboard(unsigned int user_id, unsigned int taskboard_id);
DatabaseResult kickOutUserFromTaskboard(unsigned int user_id, unsigned int taskboard_id);
DatabaseResult deleteTaskBoard(TaskBoard taskboard);

// functions about leaderboard
std::unordered_map<std::string, unsigned int> getLeaderboard();// I don't think theres a need to use DatabaseResult here, since this function will always succeed
// if you want to set xp, just call updateUserInfo
// if you want to get xp, just call getLeaderboard() and find the user. The cost are negligible
#endif