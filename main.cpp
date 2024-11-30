#include "sql/sqlite3.h"
#include <iostream>
using namespace std;

bool createDB();
bool createTable();


class Library {
private:
    sqlite3 *db;

public:
    bool createDB() {
    int res = sqlite3_open("Library.db", &db);

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

};

int main() {

    Library lib;


    if (lib.createDB()) {
        cout << "Successfully created a database";
    }

    if (lib.createTable()) {
        cout << "Successfully created a table";
    }

    return 0;
}


