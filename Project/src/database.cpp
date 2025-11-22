#include "database.h"
#include <iostream>
#include <format>
#include <list>
#include <memory>

mysqlx::Session *session;
mysqlx::Schema *db;

std::unique_ptr<mysqlx::Table> userTable, taskTable, taskboardTable, taskboardUserTable, invitationTable;

bool debug;
// if adding user requires invitation;
bool requireInvitation = true;

Task::Task(const mysqlx::Row row){
    this->taskID = row[0].get<unsigned int>();
	this->boardID = row[1].get<unsigned int>();
	this->assigned = !row[2].isNull();
	if (this->assigned) getUser(row[2].get<unsigned int>(), this->assignedUser);
	this->title = row[3].get<std::string>();
	this->type = row[4].get<std::string>();
	this->duedate = date(row[5]);
	this->startDate = date(row[6]);
	this->finished = !row[7].isNull();
	if (this->finished) this->finishedOn = date(row[7]);
	this->pinned = row[8].get<bool>();
}

TaskBoard::TaskBoard(mysqlx::Row board){
    this->taskboard_id = board[0].get<unsigned int>();
    this->name = board[1].get<std::string>();

    // get all users
    std::list<mysqlx::Row> users = taskboardUserTable->select("user_id", "isAdmin").where("board_id = :bid").bind("bid", taskboard_id).execute().fetchAll();
    for (mysqlx::Row each_user : users){
        User user;
        // we expect this to be true, since its in the database. 
        // if we cannot find that user, there must be a very severe problem
        if (getUser(each_user[0].get<unsigned int>(), user) != SUCCESS) return;

        this->users.insert(user);
        if (each_user[1].get<bool>()) this->admins.insert(user);
    }

    // get all tasks
    std::list<mysqlx::Row> tasks = taskTable->select("*").where("board_id = :bid").bind("bid", taskboard_id).execute().fetchAll();
    std::cout << "Found " << tasks.size() << " Tasks for " << this->name << std::endl;
    for (mysqlx::Row task : tasks){
        // prepare user
        User user;
        if (!task[2].isNull()) {
	        DatabaseResult res = getUser(task[2].get<unsigned int>(), user);
	        if (res != SUCCESS) return;
        }

        Task returnedTask;
        returnedTask.taskID = task[0].get<unsigned int>();
		returnedTask.boardID = task[1].get<unsigned int>();
	    returnedTask.assigned = !task[2].isNull();
        if (returnedTask.assigned) returnedTask.assignedUser = user;
	    returnedTask.title = task[3].get<std::string>();
	    returnedTask.type = task[4].get<std::string>();
	    returnedTask.duedate = date(task[5]);
	    returnedTask.startDate = date(task[6]);
	    returnedTask.finished = !task[7].isNull();
	    if (returnedTask.finished) returnedTask.finishedOn = date(task[7]);
		returnedTask.pinned = task[8].get<bool>();
        this->tasklist.insert(returnedTask);
        std::cout << "Inserted task " << returnedTask.title << '\n';
    }
    std::cout << "Found " << this->tasklist.size() << " Tasks for " << this->name << std::endl;
}

void initDatabase() {
    std::string user = MYSQL_USER;
    std::string password = MYSQL_PASSWORD;

    // wtf why this is supposed to be debug-only why I pushed it. :(
    //std::cout << "username and password: " << user << " " << password << std::endl;

    try {
        session = new mysqlx::Session("riku.shoshin.uwaterloo.ca", 33060, user, password);
        std::cout << "Connected to MySQL Server!" << std::endl;

        std::string useDBQuery = "USE ";
		session->sql(useDBQuery + MYSQL_DB_NAME).execute();
		std::cout << "Database Initialized Successfully" << std::endl;

		mysqlx::Schema db = session->getSchema(MYSQL_DB_NAME);
		
#ifdef TEST_ENV
		userTable = std::make_unique<mysqlx::Table>(db.getTable("Test_User"));
		taskTable = std::make_unique<mysqlx::Table>(db.getTable("Test_Task"));
		taskboardTable = std::make_unique<mysqlx::Table>(db.getTable("Test_TaskBoard"));
		taskboardUserTable = std::make_unique<mysqlx::Table>(db.getTable("Test_TaskBoardUser"));
		invitationTable = std::make_unique<mysqlx::Table>(db.getTable("Test_Invitation"));
		debug = true;
#else
        userTable = std::make_unique<mysqlx::Table>(db.getTable("User"));
        taskTable = std::make_unique<mysqlx::Table>(db.getTable("Task"));
        taskboardTable = std::make_unique<mysqlx::Table>(db.getTable("TaskBoard"));
        taskboardUserTable = std::make_unique<mysqlx::Table>(db.getTable("TaskBoardUser"));
        invitationTable = std::make_unique<mysqlx::Table>(db.getTable("Invitation"));
        debug = false;
#endif

    }
    catch (const mysqlx::Error& err) {
        std::cerr << "Error: " << err.what() << std::endl;
		exit(1);
    }
}

void closeDatabaseConnection() {
    session->close();
	delete session;
}

DatabaseResult registerUser(std::string username, unsigned int password){
    try{
        // test if name overflows
        if (username.length() > 50) return NAME_OVERFLOW;

        // test if we already have existing users with that name
		mysqlx::RowResult res = userTable->select("*").where("username = :username").bind("username", username).execute();
        if (res.count() != 0){
            return DUPLICATE_NAME;
        }

		userTable->insert("username", "password_hash").values(username, (int)password).execute();
        return SUCCESS;
    }catch (const mysqlx::Error& err) {
        std::cerr << "Error: " << err.what() << std::endl;
		return SQL_ERROR;
    }
}

DatabaseResult getUser(std::string username, User &returnedUser){
    try{
        // first get the encrypted username;
        if (username.length() > 50) return DOES_NOT_EXIST;

        mysqlx::RowResult res = userTable->select("*").where("username = :username").bind("username", username).execute();
        if (res.count() == 0) {
            return DOES_NOT_EXIST;
        }
        
        mysqlx::Row row = res.fetchOne();
		returnedUser.userid = row[0].get<int>();
		returnedUser.username = row[1].get<std::string>();
		returnedUser.password = row[2].get<int>();
		returnedUser.points = row[3].get<int>();

        return SUCCESS;
    }catch (const mysqlx::Error& err) {
        std::cerr << "Error: " << err.what() << std::endl;
		return SQL_ERROR;
    }
}

DatabaseResult getUser(unsigned int userid, User& returnedUser) {
    try {
        mysqlx::RowResult res = userTable->select("*").where("user_id = :userid").bind("userid", userid).execute();
        if (res.count() == 0) {
            return DOES_NOT_EXIST;
        }

        mysqlx::Row row = res.fetchOne();

        returnedUser.userid = row[0].get<int>();
        returnedUser.username = row[1].get<std::string>();
        returnedUser.password = row[2].get<int>();
        returnedUser.points = row[3].get<int>();


        return SUCCESS;
    }
    catch (const mysqlx::Error& err) {
        std::cerr << "Error: " << err.what() << std::endl;
        return SQL_ERROR;
    }
}

DatabaseResult updateUserInfo(User user) {
    try{
        // first get the encrypted username;
        if (user.username.length() > 50) return NAME_OVERFLOW;

        // test if we already have existing users with that name
        if (userTable->select("*").where("username = :username and not user_id = :uid").bind("username", user.username).bind("uid", user.userid).execute().count() > 0) {
            return DUPLICATE_NAME;
        }

        // test if user with that userid actually exist
        if (userTable->select("*").where("user_id = :uid").bind("uid", user.userid).execute().count() == 0) {
            return DOES_NOT_EXIST;
        }

		userTable->update().set("username", user.username).set("password_hash", user.password).set("xp", user.points).where("user_id = :uid").bind("uid", user.userid).execute();
        return SUCCESS;
    }catch (const mysqlx::Error& err) {
        std::cerr << "Error: " << err.what() << std::endl;
		return SQL_ERROR;
    }
}

DatabaseResult addTask(unsigned int taskboard_id, Task& task, unsigned int performed_by) {
    try {
        DatabaseResult ret = userPrivilegeCheck(performed_by, taskboard_id, false);
        if (ret != SUCCESS) return ret;
        
        // check task's name and type length
        if (task.title.length() > 100) return NAME_OVERFLOW;
		if (task.type.length() > 100) return NAME_OVERFLOW;
        
		// check time conflict
		if (task.duedate < task.startDate) return TIME_CONFLICT;
		if (task.duedate < date()) return TIME_CONFLICT;

        // check if we already have an identical task
        if (taskTable->select("*").where("board_id = :board_id and title = :title and type = :type").bind("board_id", taskboard_id).bind("title", task.title).bind("type", task.type).execute().count() != 0){
            std::cout << "Already Exist!\n" << std::endl;
            return ALREADY_EXIST;
        }

        mysqlx::Result res = taskTable->insert("board_id", "title", "type", "start_date", "due_date").values(taskboard_id, task.title, task.type, task.startDate.toString(), task.duedate.toString()).execute();
        
        task.taskID = res.getAutoIncrementValue();

        return SUCCESS;
    }
    catch (const mysqlx::Error& err) {
        std::cerr << "Error: " << err.what() << std::endl;
        return SQL_ERROR;
    }
}

DatabaseResult deleteTask(unsigned int task_id, unsigned int performed_by) {
    try {
        mysqlx::RowResult res = taskTable->select("board_id").where("task_id = :task_id").bind("task_id", task_id).execute();

		// first check if we actually have that task
		if (res.count() != 1) return DOES_NOT_EXIST;

        DatabaseResult ret = userPrivilegeCheck(performed_by, res.fetchOne()[0].get<unsigned int>(), false);
        if (ret != SUCCESS) return ret;

		taskTable->remove().where("task_id = :task_id").bind("task_id", task_id).execute();
		return SUCCESS;
	}
	catch (const mysqlx::Error& err) {
		std::cerr << "Error: " << err.what() << std::endl;
		return SQL_ERROR;
    }
}

DatabaseResult updateTask(Task task, unsigned int performed_by) {
    try {
        DatabaseResult ret = userPrivilegeCheck(performed_by, task.boardID, false);
        if (ret != SUCCESS) return ret;

        // first check if we actually have that task
        if (taskTable->select("*").where("task_id = :task_id").bind("task_id", task.taskID).execute().count() != 1) return DOES_NOT_EXIST;

        // check task's name and type length
        if (task.title.length() > 100) return NAME_OVERFLOW;
        if (task.type.length() > 100) return NAME_OVERFLOW;

        // check time conflict
        std::cout << "due: " << task.duedate.toString() << "start: " << task.startDate.toString() << "op: " << (task.duedate < task.startDate) << std::endl;
        if (task.duedate < task.startDate) return TIME_CONFLICT;

        if (task.assigned) {
            if (userTable->select("*").where("user_id = :uid").bind("uid", task.assignedUser.userid).execute().count() != 1) return DOES_NOT_EXIST;
        }

		// ensure that the task remain in the same taskboard
        // we do not support changing taskboard at this stage
		mysqlx::RowResult res = taskTable->select("board_id").where("task_id = :task_id").bind("task_id", task.taskID).execute();
		if (res.fetchOne()[0].get<unsigned int>() != task.boardID) return OTHER_ERROR;

        // update task
		taskTable->update().set("assigned_user_id", task.assigned ? (mysqlx::Value)task.assignedUser.userid : mysqlx::nullvalue)
            .set("title", task.title).set("type", task.type).set("pinned", task.pinned)
            .set("start_date", task.startDate.toString()).set("due_date", task.duedate.toString())
            .set("finished_date", task.finished ? task.finishedOn.toString() : mysqlx::nullvalue)
            .where("task_id = :task_id").bind("task_id", task.taskID).execute();
        
        return SUCCESS;
    }
    catch (const mysqlx::Error& err) {
        std::cerr << "Error: " << err.what() << std::endl;
        return SQL_ERROR;
    }
}

DatabaseResult getTask(unsigned int task_id, Task &returnedTask) {
    try {
        // first check if we actually have that task
        std::cout << "checking task " << task_id << std::endl;
        if (taskTable->select("*").where("task_id = :task_id").bind("task_id", task_id).execute().count() != 1) return DOES_NOT_EXIST;

		mysqlx::Row row = taskTable->select("*").where("task_id = :task_id").bind("task_id", task_id).execute().fetchOne();

        // prepare user
        User user;
        if (!row[2].isNull()) {
			DatabaseResult res = getUser(row[2].get<unsigned int>(), user);
			if (res != SUCCESS) return res;
        }

		returnedTask.taskID = row[0].get<unsigned int>();
		returnedTask.boardID = row[1].get<unsigned int>();
		returnedTask.assigned = !row[2].isNull();
		if (returnedTask.assigned) returnedTask.assignedUser = user;
		returnedTask.title = row[3].get<std::string>();
		returnedTask.type = row[4].get<std::string>();
		returnedTask.duedate = date(row[5]);
		returnedTask.startDate = date(row[6]);
		returnedTask.finished = !row[7].isNull();
		if (returnedTask.finished) returnedTask.finishedOn = date(row[7]);
		returnedTask.pinned = row[8].get<bool>();

		return SUCCESS;
    }
    catch (const mysqlx::Error& err) {
        std::cerr << "Error: " << err.what() << std::endl;
        return SQL_ERROR;
    }
}

DatabaseResult getAssignedTaskForUser(unsigned int user_id, std::vector<Task> &returnedTaskList){
    try {
        // test if user with that userid actually exist
        if (userTable->select("*").where("user_id = :uid").bind("uid", user_id).execute().count() == 0) {
            return DOES_NOT_EXIST;
        }

        std::list<mysqlx::Row> rows = taskTable->select("*").where("assigned_user_id = :uid").bind("uid", user_id).execute().fetchAll();
        
        returnedTaskList.clear();
        for (mysqlx::Row row : rows){
            returnedTaskList.push_back(Task(row));
        }
        
        return SUCCESS;
    }catch (const mysqlx::Error& err) {
        std::cerr << "Error: " << err.what() << std::endl;
		return SQL_ERROR;
    }
}

DatabaseResult getTaskBoardByUser(unsigned int user_id, std::vector<TaskBoard> &returnedTaskList){
    try {
        // test if user with that userid actually exist
        if (userTable->select("*").where("user_id = :uid").bind("uid", user_id).execute().count() == 0) {
            return DOES_NOT_EXIST;
        }

        std::list<mysqlx::Row> rows = taskboardUserTable->select("board_id").where("user_id = :uid").bind("uid", user_id).execute().fetchAll();;
        returnedTaskList = std::vector<TaskBoard>();
        
        for (mysqlx::Row board : rows) {
            TaskBoard tb;
            getTaskBoardByID(board[0].get<unsigned int>(), tb);
            std::cout << "Discovered " << tb.tasklist.size() << " Tasks for " << tb.name << '\n';
            returnedTaskList.push_back(tb);
        }

        return SUCCESS;
    }catch (const mysqlx::Error& err) {
        std::cerr << "Error: " << err.what() << std::endl;
		return SQL_ERROR;
    }
}

DatabaseResult createTaskBoard(unsigned int owner_id, std::string name, TaskBoard &createdTaskboard){
    if (name.length() > 100) return NAME_OVERFLOW;

    User owner;
    DatabaseResult result = getUser(owner_id, owner);
    if (result != SUCCESS) return result;
    createdTaskboard.name = name;
    createdTaskboard.tasklist = std::multiset<Task>();
    createdTaskboard.users = std::multiset<User>();
    createdTaskboard.admins = std::multiset<User>();
    createdTaskboard.users.insert(owner);
    createdTaskboard.admins.insert(owner);

    try{
        mysqlx::Result res = taskboardTable->insert("name").values(name).execute();
        createdTaskboard.taskboard_id = res.getAutoIncrementValue();

        internal_addUserToTBoard(owner_id, createdTaskboard.taskboard_id, true);

        return SUCCESS;
    }catch (const mysqlx::Error& err) {
        std::cerr << "Error: " << err.what() << std::endl;
		return SQL_ERROR;
    }
}

DatabaseResult userPrivilegeCheck(unsigned int performed_by, unsigned int taskboard_id, bool requireAdmin){
    try{
        // test if that taskboard actually exist
        if (taskboardTable->select("*").where("board_id = :bid").bind("bid", taskboard_id).execute().count() == 0) {
            return DOES_NOT_EXIST;
        }

        // test if the operator is an admin
        mysqlx::RowResult res = taskboardUserTable->select("isAdmin").where("user_id = :uid AND board_id = :bid").bind("uid", performed_by).bind("bid", taskboard_id).execute();

        if (res.count() == 0) {
            return USER_ACCESS_DENINED;
        }
        if (requireAdmin && !res.fetchOne()[0].get<bool>()) return USER_ACCESS_DENINED;

        // we are good now
        return SUCCESS;
    }catch (const mysqlx::Error& err) {
        std::cerr << "Error: " << err.what() << std::endl;
		return SQL_ERROR;
    }
}

DatabaseResult renameTaskBoard(unsigned int board_id, std::string name, unsigned int performed_by){
    try{
        if (name.length() > 100) return NAME_OVERFLOW;

        DatabaseResult isValid = userPrivilegeCheck(performed_by, board_id);
        if (isValid != SUCCESS) return isValid;
        
        taskboardTable->update().set("name", name).where("board_id = :bid").bind("bid", board_id).execute();

        return SUCCESS;
    }catch (const mysqlx::Error& err) {
        std::cerr << "Error: " << err.what() << std::endl;
		return SQL_ERROR;
    }
}

DatabaseResult updateUserStatus(unsigned int user_id, unsigned int taskboard_id, bool isAdmin, unsigned int performed_by){
    try{
        // test if user with that userid actually exist
        if (userTable->select("*").where("user_id = :uid").bind("uid", user_id).execute().count() == 0) {
            return DOES_NOT_EXIST;
        }

        DatabaseResult isValid = userPrivilegeCheck(performed_by, taskboard_id);
        if (isValid != SUCCESS) return isValid;

        // test if that user is in the task board
        if (taskboardUserTable->select("*").where("user_id = :uid AND board_id = :bid").bind("uid", user_id).bind("bid", taskboard_id).execute().count() == 0) {
            // make sure the user is invited
            if (requireInvitation && invitationTable->select("*").where("guest = :uid").bind("uid", user_id).execute().count() == 0){
                return USER_ACCESS_DENINED;
            
            }
            internal_addUserToTBoard(user_id, taskboard_id, isAdmin);
            return SUCCESS;
        }

        // make sure there is at least one admin
        if (!isAdmin && taskboardUserTable->select("*").where("board_id = :bid AND isAdmin = true").bind("bid", taskboard_id).execute().count() == 1){
            return USER_CONFLICT;
        }   

        // update value
        taskboardUserTable->update().set("isAdmin", isAdmin).where("user_id = :uid AND board_id = :bid").bind("uid", user_id).bind("bid", taskboard_id).execute();

        return SUCCESS;
    }catch (const mysqlx::Error& err) {
        std::cerr << "Error: " << err.what() << std::endl;
		return SQL_ERROR;
    }
}

DatabaseResult addUserToTaskboard(unsigned int user_id, unsigned int taskboard_id, unsigned int performed_by){
    try{
        // test if user with that userid actually exist
        if (userTable->select("*").where("user_id = :uid").bind("uid", user_id).execute().count() == 0) {
            return DOES_NOT_EXIST;
        }

        // make sure the user is invited
        if (requireInvitation && invitationTable->select("*").where("guest = :uid").bind("uid", user_id).execute().count() == 0){
            return USER_ACCESS_DENINED;
        }

        DatabaseResult isValid = userPrivilegeCheck(performed_by, taskboard_id);
        if (isValid != SUCCESS) return isValid;

        // make sure user is not in the task board
        if (taskboardUserTable->select("*").where("user_id = :uid AND board_id = :bid").bind("uid", user_id).bind("bid", taskboard_id).execute().count() != 0) {
            return ALREADY_EXIST;
        }

        // adding that user means he accepted the invitation
        invitationTable->remove().where("guest = :uid").bind("uid", user_id).execute();

        internal_addUserToTBoard(user_id, taskboard_id);
        return SUCCESS;
    }catch (const mysqlx::Error& err) {
        std::cerr << "Error: " << err.what() << std::endl;
		return SQL_ERROR;
    }
}

inline void internal_addUserToTBoard(unsigned int user_id, unsigned int taskboard_id, bool isAdmin){
    taskboardUserTable->insert("board_id", "user_id", "isAdmin").values(taskboard_id, user_id, isAdmin).execute();
}

DatabaseResult kickOutUserFromTaskboard(unsigned int user_id, unsigned int taskboard_id, unsigned int performed_by){
    try{
        // make sure user is in the task board
        if (taskboardUserTable->select("*").where("user_id = :uid AND board_id = :bid").bind("uid", user_id).bind("bid", taskboard_id).execute().count() == 0) {
            return DOES_NOT_EXIST;
        }

        // kinda lazy to create a new function for 'user leaving taskboard'. Therefore, no privilege needed if users remove themselves
        DatabaseResult isValid = userPrivilegeCheck(performed_by, taskboard_id, user_id != taskboard_id);
        if (isValid != SUCCESS) return isValid;

        // make sure the database has at least one member
        // make sure there is at least one admin
        if (taskboardUserTable->select("*").where("board_id = :bid").bind("bid", taskboard_id).execute().count() == 1){
            return USER_CONFLICT;
        }   

        taskboardUserTable->remove().where("user_id = :uid AND board_id = :bid").bind("uid", user_id).bind("bid", taskboard_id).execute();

        return SUCCESS;
    }catch (const mysqlx::Error& err) {
        std::cerr << "Error: " << err.what() << std::endl;
		return SQL_ERROR;
    }
}

DatabaseResult deleteTaskBoard(unsigned int board_id, unsigned int performed_by){
    try{
        DatabaseResult isValid = userPrivilegeCheck(performed_by, board_id);
        if (isValid != SUCCESS) return isValid;

        // bye bye
        taskboardTable->remove().where("board_id = :bid").bind("bid", board_id).execute();

        return SUCCESS;
    }catch (const mysqlx::Error& err) {
        std::cerr << "Error: " << err.what() << std::endl;
		return SQL_ERROR;
    }
}

// invite a user to join the taskboard
DatabaseResult inviteUser(unsigned int fromUser, unsigned int toUser, unsigned int taskBoard_id){
    try{
        DatabaseResult isValid = userPrivilegeCheck(fromUser, taskBoard_id);
        if (isValid != SUCCESS) return isValid;

         // test if user with that userid actually exist
        if (userTable->select("*").where("user_id = :uid").bind("uid", toUser).execute().count() == 0) {
            return DOES_NOT_EXIST;
        }

        // test if that user is already in
        if (taskboardUserTable->select("*").where("user_id = :uid AND board_id = :bid").bind("uid", toUser).bind("bid", taskBoard_id).execute().count() != 0){
            return ALREADY_EXIST;
        }

        // test if that same user is invited by the same host
        if (invitationTable->select("*").where("board_id = :bid AND host = :hst AND guest = :gst").bind("bid", taskBoard_id).bind("hst", fromUser).bind("gst", toUser).execute().count() != 0){
            return ALREADY_EXIST;
        }

        invitationTable->insert("board_id", "host", "guest").values(taskBoard_id, fromUser, toUser).execute();

        return SUCCESS;
    }catch (const mysqlx::Error& err) {
        std::cerr << "Error: " << err.what() << std::endl;
		return SQL_ERROR;
    }
} 

// reject all invitations to that board
DatabaseResult rejectInvitation(unsigned int user_id, unsigned int board_id){
    try{
        invitationTable->remove().where("board_id = :bid and guest = :usr").bind("usr", user_id).bind("bid", "board_id").execute();

        return SUCCESS;
    }catch (const mysqlx::Error& err) {
        std::cerr << "Error: " << err.what() << std::endl;
		return SQL_ERROR;
    }
}

// returns all invitations the user "whom" has received
// the key of the dictionary is "who invits you", and the value of the dictionary is "to what taskboard"
// there is no "accept invitation" thing, you just directly updtae the taskboard, thats enough
DatabaseResult getAllInvitation(unsigned int user_id, std::unordered_multimap<User, TaskBoard, UserHasher> &returnedInvitations){
    try{
        mysqlx::RowResult result = invitationTable->select("*").where("guest = :uid").bind("uid", user_id).execute();
        
        returnedInvitations = std::unordered_multimap<User, TaskBoard, UserHasher>();
        for (mysqlx::Row row : result.fetchAll()){
            User usr;
            TaskBoard board;
            getUser(row[1].get<unsigned int>(), usr);
            getTaskBoardByID(row[0].get<unsigned int>(), board);

            returnedInvitations.insert({usr, board});
        }

        return SUCCESS;
    }catch (const mysqlx::Error& err) {
        std::cerr << "Error: " << err.what() << std::endl;
		return SQL_ERROR;
    }
}

DatabaseResult getTaskBoardByID(unsigned int board_id, TaskBoard& returnedTaskBoard){
    try{
        mysqlx::RowResult res = taskboardTable->select("*").where("board_id = :bid").bind("bid", board_id).execute();
        if (res.count() != 1) return DOES_NOT_EXIST;

        returnedTaskBoard = TaskBoard(res.fetchOne());
        return SUCCESS;
    }catch (const mysqlx::Error& err) {
        std::cerr << "Error: " << err.what() << std::endl;
		return SQL_ERROR;
    }
}

std::unordered_map<std::string, unsigned int> getLeaderboard() {
    try{
		std::list<mysqlx::Row> rows = userTable->select("username", "xp").execute().fetchAll();
        
        std::unordered_map<std::string, unsigned int> leaderboard;
        for (mysqlx::Row row : rows){
            leaderboard[row[0].get<std::string>()] = row[1].get<unsigned int>();
        }
        
        return leaderboard;
    }catch (const mysqlx::Error& err) {
        std::cerr << "Error: " << err.what() << std::endl;
		return std::unordered_map<std::string, unsigned int>();
    }
}

#ifndef TEST_ENV
#ifdef DATABASE_TEST
int main() {
	initDatabase();
    std::cout << "result: " << registerUser("long password", 2147483699) << std::endl;
    User usr;
    std::cout << "result: "  << getUser("long password", usr) << std::endl;
    std::cout << "hash: " << usr.password << std::endl;
	closeDatabaseConnection();
	return 0;
}
#endif
#endif
