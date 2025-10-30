#include <mysqlx/xdevapi.h>
#include <iostream>

int main() {
    try {
        mysqlx::Session sess("riku.shoshin.uwaterloo.ca", 33060, "s969chen", "Welcome1!");

        std::cout << "Connected to MySQL Server!" << std::endl;

        mysqlx::Schema db = sess.getSchema("se101_s969chen");
        mysqlx::Table table = db.getTable("Foo");

        mysqlx::RowResult res = table.select("A").execute();
        for (mysqlx::Row row : res)
            std::cout << "User: " << row[0].get<std::string>() << std::endl;

        sess.close();
    }
    catch (const mysqlx::Error& err) {
        std::cerr << "Error: " << err.what() << std::endl;
    }
}