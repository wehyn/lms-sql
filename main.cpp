#include "sql/sqlite3.h"
#include <iostream>
using namespace std;


sqlite3 *db;

int main() {

    int res = sqlite3_open("test.db", &db);

    if (res != SQLITE_OK) {
        cout << "Error in creating DB: " << sqlite3_errmsg(db) << endl;
        return 1;
    }
    else {
        cout << "Database File creation successful." << endl;
    }

    // Creates a table for book
    string sql = "CREATE TABLE IF NOT EXISTS BOOK ("
        "ID INT PRIMARY KEY NOT NULL, "
        "TITLE TEXT NOT NULL, "
        "AUTHOR TEXT NOT NULL,"
        "ISBN INT NOT NULL, "
        "publicationDATE TEXT NOT NULL,"
        "availableCopies INT NOT NULL);";

    sqlite3_close(db);
    return 0;
}

