#include <mysqlx/xdevapi.h>
#include "database.h"
#include <iostream>

mysqlx::Session *session;
mysqlx::Schema *db;

void initDatabase() {
    std::string user = MYSQL_USER;
    std::string password = MYSQL_PASSWORD;

    std::cout << "username and password: " << user << " " << password << std::endl;

    try {
        session = new mysqlx::Session("riku.shoshin.uwaterloo.ca", 33060, user, password);
        std::cout << "Connected to MySQL Server!" << std::endl;

        //db = &session->getSchema(MYSQL_DB_NAME);
		//std::cout << "Retrieved DB" << std::endl;
        std::string useDBQuery = "USE ";
		std::string dbName = MYSQL_DB_NAME;
		session->sql(useDBQuery + dbName).execute();
        mysqlx::SqlResult res = session->sql("select * from User").execute();
        mysqlx::Row row = res.fetchOne();
        std::cout << "userid: " << row.get(0).get<unsigned int>() << "\nusername: " << row.get(1).get<std::string>() << std::endl;

		std::cout << "Initialized Tables" << std::endl;
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

int main() {
	initDatabase();
	closeDatabaseConnection();
	return 0;
}