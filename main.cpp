#include "sql/sqlite3.h"
#include <iostream>
#include <string>
#include <vector>
#include <iomanip>
using namespace std;

class Library {
private:
    sqlite3 *db;

    // Helper function to display table header
     // Helper function to display table header
    void printTableHeader(const vector<string>& headers) {
        cout << string(125, '-') << endl;
        for (const auto& header : headers) {
            cout << left << setw(20) << header;
        }
        cout << endl;
        cout << string(125, '-') << endl;
    }

    // Callback function for listing books
    static int bookCallback(void* data, int argc, char** argv, char** colName) {
        vector<string>* results = static_cast<vector<string>*>(data);
        for (int i = 0; i < argc; i++) {
            results->push_back(argv[i] ? argv[i] : "NULL");
        }
        return 0;
    }

    // Callback function for listing authors
    static int authorCallback(void* data, int argc, char** argv, char** colName) {
        vector<string>* results = static_cast<vector<string>*>(data);
        for (int i = 0; i < argc; i++) {
            results->push_back(argv[i] ? argv[i] : "NULL");
        }
        return 0;
    }

    // Callback function for listing members
    static int memberCallback(void* data, int argc, char** argv, char** colName) {
        vector<string>* results = static_cast<vector<string>*>(data);
        for (int i = 0; i < argc; i++) {
            results->push_back(argv[i] ? argv[i] : "NULL");
        }
        return 0;
    }


public:
    // Constructor
    Library() {
        db = NULL;
    }

    // Destructor to close database connection
    ~Library() {
        if (db) {
            sqlite3_close(db);
        }
    }

    // Already existing methods from previous file
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

        // Creates tables for Book, Author, and Member
        string sqlBook = "CREATE TABLE IF NOT EXISTS BOOK ("
            "ID INTEGER PRIMARY KEY AUTOINCREMENT, "
            "TITLE TEXT NOT NULL, "
            "AUTHOR TEXT NOT NULL,"
            "ISBN TEXT NOT NULL UNIQUE, "
            "publicationDATE TEXT NOT NULL,"
            "availableCopies INT NOT NULL);";

        string sqlAuthor = "CREATE TABLE IF NOT EXISTS AUTHOR ("
            "ID INTEGER PRIMARY KEY AUTOINCREMENT, "
            "NAME TEXT NOT NULL UNIQUE, "
            "BIOGRAPHY TEXT);";

        string sqlMember = "CREATE TABLE IF NOT EXISTS MEMBER ("
            "ID INTEGER PRIMARY KEY AUTOINCREMENT, "
            "NAME TEXT NOT NULL, "
            "CONTACT_INFO TEXT);";

        int resBook = sqlite3_exec(db, sqlBook.c_str(), NULL, 0, &err);
        int resAuthor = sqlite3_exec(db, sqlAuthor.c_str(), NULL, 0, &err);
        int resMember = sqlite3_exec(db, sqlMember.c_str(), NULL, 0, &err);

        if (resBook != SQLITE_OK || resAuthor != SQLITE_OK || resMember != SQLITE_OK) {
            cout << "Error in creating table: " << err << endl;
            sqlite3_free(err);
            return false;
        }

        cout << "Tables created successfully." << endl;
        return true;
    }

    // Callback function for listing data
    static int callback(void* data, int argc, char** argv, char** colName) {
        for (int i = 0; i < argc; i++) {
            cout << colName[i] << ": " << (argv[i] ? argv[i] : "NULL") << " ";
        }
        cout << endl;
        return 0;
    }

    // Add a new book
    bool addBook(const string& title, const string& author, 
                 const string& isbn, const string& publicationDate, 
                 int availableCopies) {
        char* err;
        string sql = "INSERT INTO BOOK (TITLE, AUTHOR, ISBN, publicationDATE, availableCopies) "
                     "VALUES ('" + title + "', '" + author + "', '" + 
                     isbn + "', '" + publicationDate + "', " + 
                     to_string(availableCopies) + ");";

        int res = sqlite3_exec(db, sql.c_str(), NULL, 0, &err);

        if (res != SQLITE_OK) {
            cout << "Error inserting book: " << err << endl;
            sqlite3_free(err);
            return false;
        }
        
        cout << "Book added successfully." << endl;
        return true;
    }

    // Remove a book by ISBN
    bool removeBook(const string& isbn) {
        char* err;
        string sql = "DELETE FROM BOOK WHERE ISBN = '" + isbn + "';";

        int res = sqlite3_exec(db, sql.c_str(), NULL, 0, &err);

        if (res != SQLITE_OK) {
            cout << "Error removing book: " << err << endl;
            sqlite3_free(err);
            return false;
        }
        
        cout << "Book removed successfully." << endl;
        return true;
    }

    // List all books
  void listBooks() {
        char* err;
        vector<string> results;
        string sql = "SELECT * FROM BOOK;";

        int res = sqlite3_exec(db, sql.c_str(), bookCallback, &results, &err);

        if (res != SQLITE_OK) {
            cout << "Error listing books: " << err << endl;
            sqlite3_free(err);
            return;
        }

        // Book headers
        vector<string> headers;
        headers.push_back("ID");
        headers.push_back("Title");
        headers.push_back("Author");
        headers.push_back("ISBN");
        headers.push_back("Publication Date");
        headers.push_back("Available Copies");
        
        printTableHeader(headers);

        // Print books in table format
        for (size_t i = 0; i < results.size(); i += headers.size()) {
            for (size_t j = 0; j < headers.size(); ++j) {
                cout << left << setw(20) << results[i + j];
            }
            cout << endl;
        }
        cout << string(125, '-') << endl;
    }

    // Add an author
    bool addAuthor(const string& name, const string& biography = "") {
        char* err;
        string sql = "INSERT INTO AUTHOR (NAME, BIOGRAPHY) "
                     "VALUES ('" + name + "', '" + biography + "');";

        int res = sqlite3_exec(db, sql.c_str(), NULL, 0, &err);

        if (res != SQLITE_OK) {
            cout << "Error adding author: " << err << endl;
            sqlite3_free(err);
            return false;
        }
        
        cout << "Author added successfully." << endl;
        return true;
    }

     void listAuthors() {
        char* err;
        vector<string> results;
        string sql = "SELECT * FROM AUTHOR;";

        int res = sqlite3_exec(db, sql.c_str(), authorCallback, &results, &err);

        if (res != SQLITE_OK) {
            cout << "Error listing authors: " << err << endl;
            sqlite3_free(err);
            return;
        }

        // Author headers
        vector<string> headers;
        headers.push_back("ID");
        headers.push_back("Name");
        headers.push_back("Biography");
        
        printTableHeader(headers);

        // Print authors in table format
        for (size_t i = 0; i < results.size(); i += headers.size()) {
            for (size_t j = 0; j < headers.size(); ++j) {
                cout << left << setw(20) << results[i + j];
            }
            cout << endl;
        }
        cout << string(125, '-') << endl;
    }


    // Add a member
    bool addMember(const string& name, const string& contactInfo = "") {
        char* err;
        string sql = "INSERT INTO MEMBER (NAME, CONTACT_INFO) "
                     "VALUES ('" + name + "', '" + contactInfo + "');";

        int res = sqlite3_exec(db, sql.c_str(), NULL, 0, &err);

        if (res != SQLITE_OK) {
            cout << "Error adding member: " << err << endl;
            sqlite3_free(err);
            return false;
        }
        
        cout << "Member added successfully." << endl;
        return true;
    }

    // List all members
   void listMembers() {
        char* err;
        vector<string> results;
        string sql = "SELECT * FROM MEMBER;";

        int res = sqlite3_exec(db, sql.c_str(), memberCallback, &results, &err);

        if (res != SQLITE_OK) {
            cout << "Error listing members: " << err << endl;
            sqlite3_free(err);
            return;
        }

        // Member headers
        vector<string> headers;
        headers.push_back("ID");
        headers.push_back("Name");
        headers.push_back("Contact Info");
        
        printTableHeader(headers);

        // Print members in table format
        for (size_t i = 0; i < results.size(); i += headers.size()) {
            for (size_t j = 0; j < headers.size(); ++j) {
                cout << left << setw(20) << results[i + j];
            }
            cout << endl;
        }
        cout << string(125, '-') << endl;
    }

    // Interactive Menu
    void displayMenu() {
        int choice;
        do {
            cout << "\n--- Library Management System ---\n";
            cout << "1. Book Operations\n";
            cout << "2. Author Operations\n";
            cout << "3. Member Operations\n";
            cout << "4. Exit\n";
            cout << "Enter your choice: ";
            cin >> choice;

            switch(choice) {
                case 1: {
                    int bookChoice;
                    cout << "\nBook Operations:\n";
                    cout << "1. Add Book\n";
                    cout << "2. Remove Book\n";
                    cout << "3. List Books\n";
                    cout << "Enter your choice: ";
                    cin >> bookChoice;

                    string title, author, isbn, publicationDate;
                    int availableCopies;
                    switch(bookChoice) {
                        case 1:
                            cout << "Enter Book Title: ";
                            cin.ignore();
                            getline(cin, title);
                            cout << "Enter Author: ";
                            getline(cin, author);
                            cout << "Enter ISBN: ";
                            getline(cin, isbn);
                            cout << "Enter Publication Date: ";
                            getline(cin, publicationDate);
                            cout << "Enter Available Copies: ";
                            cin >> availableCopies;
                            addBook(title, author, isbn, publicationDate, availableCopies);
                            break;
                        case 2:
                            cout << "Enter ISBN to remove: ";
                            cin >> isbn;
                            removeBook(isbn);
                            break;
                        case 3:
                            listBooks();
                            break;
                    }
                    break;
                }
                case 2: {
                    int authorChoice;
                    cout << "\nAuthor Operations:\n";
                    cout << "1. Add Author\n";
                    cout << "2. List Authors\n";
                    cout << "Enter your choice: ";
                    cin >> authorChoice;

                    string name, biography;
                    switch(authorChoice) {
                        case 1:
                            cout << "Enter Author Name: ";
                            cin.ignore();
                            getline(cin, name);
                            cout << "Enter Author Biography (optional): ";
                            getline(cin, biography);
                            addAuthor(name, biography);
                            break;
                        case 2:
                            listAuthors();
                            break;
                    }
                    break;
                }
                case 3: {
                    int memberChoice;
                    cout << "\nMember Operations:\n";
                    cout << "1. Add Member\n";
                    cout << "2. List Members\n";
                    cout << "Enter your choice: ";
                    cin >> memberChoice;

                    string name, contactInfo;
                    switch(memberChoice) {
                        case 1:
                            cout << "Enter Member Name: ";
                            cin.ignore();
                            getline(cin, name);
                            cout << "Enter Contact Info (optional): ";
                            getline(cin, contactInfo);
                            addMember(name, contactInfo);
                            break;
                        case 2:
                            listMembers();
                            break;
                    }
                    break;
                }
                case 4:
                    cout << "Exiting Library Management System.\n";
                    break;
                default:
                    cout << "Invalid choice. Please try again.\n";
            }
        } while (choice != 4);
    }
};

int main() {
    Library lib;

    if (lib.createDB()) {
        cout << "Successfully created database\n";
    }

    if (lib.createTable()) {
        cout << "Successfully created tables\n";
    }

    lib.displayMenu();

    return 0;
}