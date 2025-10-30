#include <mysqlx/xdevapi.h>
#include "database.h"
#include <iostream>

mysqlx::Session *session;
mysqlx::Schema *db;

mysqlx::Table *userTable, *taskTable, *taskboardTable, *taskboardUserTable;

void initDatabase() {
    std::string user = MYSQL_USER;
    std::string password = MYSQL_PASSWORD;

    std::cout << "username and password: " << user << " " << password << std::endl;

    try {
        session = new mysqlx::Session("riku.shoshin.uwaterloo.ca", 33060, user, password);
        std::cout << "Connected to MySQL Server!" << std::endl;

        *db = session->getSchema(MYSQL_DB_NAME);
        mysqlx::Table table = db->getTable("Foo");

        mysqlx::RowResult res = table.select("A").execute();
        for (mysqlx::Row row : res)
            std::cout << "User: " << row[0].get<std::string>() << std::endl;

        session->close();
    }
    catch (const mysqlx::Error& err) {
        std::cerr << "Error: " << err.what() << std::endl;
    }
}

int main() {
	initDatabase();
	return 0;
}