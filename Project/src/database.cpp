#include <mysqlx/xdevapi.h>
#include "database.h"
#include <iostream>
#include <format>
#include <list>
#include <memory>

mysqlx::Session *session;
mysqlx::Schema *db;

std::unique_ptr<mysqlx::Table> userTable, taskTable, taskboardTable, taskboardUserTable, invitationTable;

User::User(unsigned int userid, std::string username, unsigned int password, unsigned int xpPoint){
    this->username = username;
    this->password = password;
    this->userid = userid;
    this->points = xpPoint;
}

Task::Task(std::string title, std::string type, time_t startDate, time_t duedate){
    this->title = title;
    this->type = type;
    this->startDate = startDate;
    this->duedate = duedate;
    // initialized but garbage value
    this->assignedUser = User();
}

Task::Task(std::string title, std::string type, unsigned int taskID, bool pinned, time_t startDate, time_t duedate, time_t finished, bool isFinished, bool isAssigned, User assignedUser){
    this->title = title;
    this->type = type;
    this->taskID = taskID;
    this->pinned = pinned;
    this->startDate = startDate;
    this->duedate = duedate;
    this->finished = isFinished;
    this->finishedOn = finished;
    this->assigned = isAssigned;
    this->assignedUser = assignedUser;
}

void initDatabase() {
    std::string user = MYSQL_USER;
    std::string password = MYSQL_PASSWORD;

    std::cout << "username and password: " << user << " " << password << std::endl;

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
#else
        userTable = std::make_unique<mysqlx::Table>(db.getTable("User"));
        taskTable = std::make_unique<mysqlx::Table>(db.getTable("Task"));
        taskboardTable = std::make_unique<mysqlx::Table>(db.getTable("Taskboard"));
        taskboardUserTable = std::make_unique<mysqlx::Table>(db.getTable("TaskboardUser"));
        invitationTable = std::make_unique<mysqlx::Table>(db.getTable("Invitation"));
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

DatabaseResult getUser(std::string username, User **returnedUser){
    try{
        // first get the encrypted username;
        if (username.length() > 50) return DOES_NOT_EXIST;

        mysqlx::RowResult res = userTable->select("*").where("username = :username").bind("username", username).execute();
        if (res.count() == 0) {
            return DOES_NOT_EXIST;
        }
        
        mysqlx::Row row = res.fetchOne();

		*returnedUser = new User(row[0].get<int>(), row[1].get<std::string>(), row[2].get<int>(), row[3].get<int>());
        return SUCCESS;
    }catch (const mysqlx::Error& err) {
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

int main() {
	initDatabase();
    User *user = new User();
	auto map = getLeaderboard();
	for (auto const& pair : map) {
		std::cout << "Username and xp: " << pair.first << " and " << pair.second << std::endl;
	}
	closeDatabaseConnection();
	return 0;
}