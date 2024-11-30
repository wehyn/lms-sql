#include "sql/sqlite3.h"
#include <iostream>
using namespace std;


sqlite3 *db;

int main() {

    if (createDB()) {
        cout << "Successfully created a database";
    }

    if (createTable()) {
        cout << "Successfully created a table";
    }

    sqlite3_close(db);
    return 0;
}

bool createDB() {
    int res = sqlite3_open("test.db", &db);

    if (res != SQLITE_OK) {
        cout << "Error in creating DB: " << sqlite3_errmsg(db) << endl;
        return false;
    }
    else {
        cout << "Database File creation successful." << endl;
        return true;
    }
}

bool createTable() {
    char *err;

    // Creates a table for book
    string sql = "CREATE TABLE IF NOT EXISTS BOOK ("
        "ID INT PRIMARY KEY NOT NULL, "
        "TITLE TEXT NOT NULL, "
        "AUTHOR TEXT NOT NULL,"
        "ISBN INT NOT NULL, "
        "publicationDATE TEXT NOT NULL,"
        "availableCopies INT NOT NULL);";

    int res = sqlite3_exec(db, sql.c_str(), NULL, 0, &err);

    if (res != SQLITE_OK) {
        cout << "Error in creating table: " << err << endl;
        sqlite3_free(err);
        return false;
    }
    else {
        cout << "Table created successfully." << endl;
        return true;
    }
}