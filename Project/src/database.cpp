#include <mysqlx/xdevapi.h>
#include "database.h"
#include <iostream>
#include <format>
#include <list>
#include <memory>

mysqlx::Session *session;
mysqlx::Schema *db;

std::unique_ptr<mysqlx::Table> userTable, taskTable, taskboardTable, taskboardUserTable, invitationTable;

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
		taskboardTable = std::make_unique<mysqlx::Table>(db.getTable("Test_Taskboard"));
		taskboardUserTable = std::make_unique<mysqlx::Table>(db.getTable("Test_TaskboardUser"));
		invitationTable = std::make_unique<mysqlx::Table>(db.getTable("Test_Invitation"));
		bool debug = true;
#else
        userTable = std::make_unique<mysqlx::Table>(db.getTable("User"));
        taskTable = std::make_unique<mysqlx::Table>(db.getTable("Task"));
        taskboardTable = std::make_unique<mysqlx::Table>(db.getTable("Taskboard"));
        taskboardUserTable = std::make_unique<mysqlx::Table>(db.getTable("TaskboardUser"));
        invitationTable = std::make_unique<mysqlx::Table>(db.getTable("Invitation"));
        bool debug = false;
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

		userTable->insert("username", "password_hash").values(username, password).execute();
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

DatabaseResult addTask(unsigned int taskboard_id, Task task) {
    try {
        // first see if that taskboard exist;
        if (taskboardTable->select("*").where("board_id = :board_id").bind("board_id", taskboard_id).execute().count() != 0) return ALREADY_EXIST;
        
        // check task's name and type length
        if (task.title.length() > 100) return NAME_OVERFLOW;
		if (task.type.length() > 100) return NAME_OVERFLOW;
        
		// check time conflict
		if (task.duedate < task.startDate) return TIME_CONFLICT;
		if (task.duedate < date()) return TIME_CONFLICT;

        taskTable->insert("board_id", "title", "type", "start_date", "finished_on").values(taskboard_id, task.title, task.type, task.startDate.toString(), task.finishedOn.toString()).execute();
        
        return SUCCESS;
    }
    catch (const mysqlx::Error& err) {
        std::cerr << "Error: " << err.what() << std::endl;
        return SQL_ERROR;
    }
}

DatabaseResult deleteTask(unsigned int task_id, unsigned int performed_by) {
    try {

		// Todo: we should check if the user has admin right to delete the task
		// However, this is too complicated for now, so we skip this step
		// We will implement this when we have taskboard admin system

		// first check if we actually have that task
		if (taskTable->select("*").where("task_id = :task_id").bind("task_id", task_id).execute().count() != 1) return DOES_NOT_EXIST;

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

        // Todo: we should check if the user has admin right to delete the task
        // However, this is too complicated for now, so we skip this step
        // We will implement this when we have taskboard admin system

        // first check if we actually have that task
        if (taskTable->select("*").where("task_id = :task_id").bind("task_id", task.taskID).execute().count() != 1) return DOES_NOT_EXIST;

        // first see if that taskboard exist;
        if (taskboardTable->select("*").where("board_id = :board_id").bind("board_id", task.boardID).execute().count() != 1) return DOES_NOT_EXIST;

        // check task's name and type length
        if (task.title.length() > 100) return NAME_OVERFLOW;
        if (task.type.length() > 100) return NAME_OVERFLOW;

        // check time conflict
        if (task.duedate < task.startDate) return TIME_CONFLICT;
        if (task.duedate < date()) return TIME_CONFLICT;

		// check if assigned user is valid
        if (task.assigned) {
            if (userTable->select("*").where("user_id = :uid").bind("uid", task.assignedUser.userid).execute().count() != 1) return DOES_NOT_EXIST;
        }

        // check if finished date is valid
        if (task.finished) {
            if (task.finishedOn < task.startDate) return TIME_CONFLICT;
			if (task.finishedOn < date()) return TIME_CONFLICT;
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
		returnedTask.duedate = date(row[5].get<std::string>());
		returnedTask.startDate = date(row[6].get<std::string>());
		returnedTask.finished = !row[7].isNull();
		if (returnedTask.finished) returnedTask.finishedOn = date(row[7].get<std::string>());
		returnedTask.pinned = row[8].get<bool>();

		return SUCCESS;
    }
    catch (const mysqlx::Error& err) {
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

int main() {
	initDatabase();
    User user = User();
	auto map = getLeaderboard();
	for (auto const& pair : map) {
		std::cout << "Username and xp: " << pair.first << " and " << pair.second << std::endl;
	}
	closeDatabaseConnection();
	return 0;
}

#endif