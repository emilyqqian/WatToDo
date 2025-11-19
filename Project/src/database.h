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
    USER_CONFLICT, // thrown if there is no admin in a taskboard
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
DatabaseResult inviteUser(unsigned int fromUser, unsigned int toUser, unsigned int taskBoard_id); 
// returns all invitations the user "whom" has received
// the key of the dictionary is "who invits you", and the value of the dictionary is "to what taskboard"
// there is no "accept invitation" thing, you just directly updtae the taskboard, thats enough
DatabaseResult getAllInvitation(unsigned int user_id, std::unordered_multimap<User, TaskBoard, UserHasher> &returnedInvitations);
DatabaseResult rejectInvitation(unsigned int user_id, unsigned int board_id);

// functions about task

// here is how it works:
// first, you somehow get the id of the taskboard where you want to add this task.
// second, you create the task with only `title`, `type`, `startDate`, and `duedate`
// third, call this function to add the task to the database
// lastly, you may notice that the task is passed by reference.
// after executing this task, I will fill the `task_id` field of the task, 
// so you can do more things using ths id. 
DatabaseResult addTask(unsigned int taskboard_id, Task &task, unsigned int performed_by);

DatabaseResult deleteTask(unsigned int task_id, unsigned int performed_by);
DatabaseResult updateTask(Task task, unsigned int performed_by);
DatabaseResult getTask(unsigned int task_id, Task &returnedTask);
DatabaseResult getAssignedTaskForUser(unsigned int user_id, std::vector<Task> &returnedTaskList);

// functions about taskBoard

DatabaseResult getTaskBoardByID(unsigned int board_id, TaskBoard& returnedTaskBoard);
// question:
// @yash, when user_id does not exist, do I return an empty list or do i report error?
// rn im being lazy, which means I would return an empty list
DatabaseResult getTaskBoardByUser(unsigned int user_id, std::vector<TaskBoard> &returnedTaskList);
DatabaseResult createTaskBoard(unsigned int owner_id, std::string name, TaskBoard &createdTaskBoard);
DatabaseResult renameTaskBoard(unsigned int board_id, std::string name, unsigned int performed_by);
DatabaseResult updateUserStatus(unsigned int user_id, unsigned int taskboard_id, bool isAdmin, unsigned int performed_by);
DatabaseResult addUserToTaskboard(unsigned int user_id, unsigned int taskboard_id, unsigned int performed_by);
DatabaseResult kickOutUserFromTaskboard(unsigned int user_id, unsigned int taskboard_id, unsigned int performed_by);
DatabaseResult deleteTaskBoard(unsigned int board_id, unsigned int performed_by);


// helper functions

// this function checks if a user has the privilege to modify the taskboard
// if any error occurs, for example, if a user does not exist or the taskboard does not exist
// it will also report
DatabaseResult userPrivilegeCheck(unsigned int performed_by, unsigned int taskboard_id, bool requireAdmin = true);
// this function does not check any pre-conditions, and does not have any error handling
// However, it is much more efficient
// use this only after you have checked that the request is valid
void internal_addUserToTBoard(unsigned int user_id, unsigned int taskboard_id, bool isAdmin = false);

// functions about leaderboard
std::unordered_map<std::string, unsigned int> getLeaderboard();// I don't think theres a need to use DatabaseResult here, since this function will always succeed
// if you want to set xp, just call updateUserInfo
// if you want to get xp, just call getLeaderboard() and find the user. The cost are negligible
#endif
