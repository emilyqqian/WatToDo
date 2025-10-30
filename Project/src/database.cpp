#include <mysqlx/xdevapi.h>
#include "database.h"
#include <iostream>
#include <format>
#include <list>

mysqlx::Session *session;
mysqlx::Schema *db;

#ifdef TEST_ENVIRONMENT
std::string prefix = "TEST_";
#else
std::string prefix = "";
#endif

std::string replaceAll(std::string str, const std::string& from, const std::string& to) {
    size_t start_pos = 0;
    while((start_pos = str.find(from, start_pos)) != std::string::npos) {
        str.replace(start_pos, from.length(), to);
        start_pos += to.length(); // Handles case where 'to' is a substring of 'from'
    }
    return str;
}

bool encrypt(std::string &str, int maxLength){
    if (str.length() > maxLength) return false;
    str = replaceAll(str, "\"", "\\\"");
    return true;
}

std::string decrypt(std::string str){
    return replaceAll(str, "\\\"", "\"");
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
            return DUPLICATE_NAME;
        }

        session->sql(std::format("insert into {}User (username, password_hash) values (\"{}\", {})", prefix, username, password)).execute();
        return SUCCESS;
    }catch (const mysqlx::Error& err) {
        std::cerr << "Error: " << err.what() << std::endl;
		return SQL_ERROR;
    }
}

DatabaseResult getUser(std::string username, User *returnedUser){
    try{
        // first get the encrypted username;
        if (!encrypt(username, 50)) return DOES_NOT_EXIST;

        mysqlx::SqlResult result = session->sql(std::format("select * from {}User where username = \"{}\"", prefix, username)).execute();

        if (!result.hasData()) return DOES_NOT_EXIST;
        
        mysqlx::Row row = result.fetchOne();
        returnedUser = new User(row[0].get<unsigned int>(), decrypt(row[1].get<std::string>()), row[2].get<unsigned int>(), row[3].get<unsigned int>());
        return SUCCESS;
    }catch (const mysqlx::Error& err) {
        std::cerr << "Error: " << err.what() << std::endl;
		return SQL_ERROR;
    }
}

DatabaseResult updateUserInfo(User user){
    try{
        // first get the encrypted username;
        if (!encrypt(user.username, 50)) return NAME_OVERFLOW;

        // test if we already have existing users with that name
        if (session->sql(std::format("select * from {}User where username = \"{}\"", prefix, user.username)).execute().hasData()){
            return DUPLICATE_NAME;
        }

        // test if user with that userid actually exist
        if (!session->sql(std::format("select * from {}User where user_id = \"{}\"", prefix, user.userid)).execute().hasData()){
            return DOES_NOT_EXIST;
        }

        session->sql(std::format("update {}User set username = \"{}\", password_hash = {}, xp = {} where user_id = {}", prefix, user.username, user.password, user.points, user.userid)).execute();
        return SUCCESS;
    }catch (const mysqlx::Error& err) {
        std::cerr << "Error: " << err.what() << std::endl;
		return SQL_ERROR;
    }
}

std::unordered_map<std::string, unsigned int> getLeaderboard(){
    try{
        std::list<mysqlx::Row> rows =  session->sql(std::format("select username, xp from {}User", prefix)).execute().fetchAll();
        
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
	closeDatabaseConnection();
	return 0;
}