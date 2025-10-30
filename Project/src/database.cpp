#include <mysqlx/xdevapi.h>
#include "database.h"
#include <iostream>
#include <format>

mysqlx::Session *session;
mysqlx::Schema *db;

#ifdef TEST_ENVIRONMENT
std::string prefix = "TEST_";
#else
std::string prefix = "";
#endif

bool encrypt(std::string &str, int maxLength){
    if (str.length > maxLength) return false;
    str.replace("\"", "\\\"");
    return true;
}

std::string decrypt(std::string str){
    return str.replace("\\\"", "\"");
}

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
		std::string dbName = MYSQL_DB_NAME;
		session->sql(useDBQuery + dbName).execute();
		std::cout << "Database Initialized Successfully" << std::endl;
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
        if (!encrypt(username, 50)) return NAME_OVERFLOW;

        // test if we already have existing users with that name
        if (session->sql(std::format("select * from {}User where username = \"{}\"", prefix, username)).execute().hasData()){
            return DUPLICATE_NAME
        }

        session->sql(std::format("insert into {}User (username, password_hash) values (\"{}\", {]})", prefix, username, password)).execute();
    }catch (const mysqlx::Error& err) {
        std::cerr << "Error: " << err.what() << std::endl;
		return SQL_ERROR;
    }
}

DatabaseResult getUser(std::string username, User &returnedUser){
    try{
        // first get the encrypted username;
        if (!encrypt(username, 50)) return DOES_NOT_EXIST;

        mysqlx::SqlResult result = session->sql(std::format("select * from {}User where username = \"{}\"", prefix, username)).execute();

        if (!result.hasData()) return DOES_NOT_EXIST;
        
        Row row = result.fetchOne();
        returnedUser = new User(row[0].get<unsigned int>(), decrypt(row[1].get<std::string>()), row[2].get(unsigned int), row[3].get<unsigned int>());
        return SUCCESS;
    }catch (const mysqlx::Error& err) {
        std::cerr << "Error: " << err.what() << std::endl;
		return SQL_ERROR;
    }
}

DatabaseResult updateUserInfo(User user){
    try{
        // first get the encrypted username;
        if (!encrypt(user.username, 50)) return OVERFLOW;

        // test if we already have existing users with that name
        if (session->sql(std::format("select * from {}User where username = \"{}\"", prefix, user.username)).execute().hasData()){
            return DUPLICATE_NAME;
        }

        // test if user with that userid actually exist
        if (!session->sql(std::format("select * from {}User where user_id = \"{}\"", prefix, user.userid)).execute().hasData()){
            return DOES_NOT_EXIST;
        }

        session->sql(std::formate("update {}User set username = \"{}\", password_hash = {}, xp = {} where user_id = {}", prefix, user.username, user.password, user.points, user.userid)).execute();
        return SUCCESS;
    }catch (const mysqlx::Error& err) {
        std::cerr << "Error: " << err.what() << std::endl;
		return SQL_ERROR;
    }
}

int main() {
	initDatabase();
	closeDatabaseConnection();
	return 0;
}