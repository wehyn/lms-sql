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

        string sqlBorrowedBooks = "CREATE TABLE IF NOT EXISTS BORROWED_BOOKS ("
            "ID INTEGER PRIMARY KEY AUTOINCREMENT, "
            "MEMBER_ID INTEGER, "
            "BOOK_ISBN TEXT, "
            "BORROW_DATE TEXT, "
            "RETURN_DATE TEXT,"
            "FOREIGN KEY(MEMBER_ID) REFERENCES MEMBER(ID), "
            "FOREIGN KEY(BOOK_ISBN) REFERENCES BOOK(ISBN));";

        int resBook = sqlite3_exec(db, sqlBook.c_str(), NULL, 0, &err);
        int resAuthor = sqlite3_exec(db, sqlAuthor.c_str(), NULL, 0, &err);
        int resMember = sqlite3_exec(db, sqlMember.c_str(), NULL, 0, &err);
        int resBorrowed = sqlite3_exec(db, sqlBorrowedBooks.c_str(), NULL, 0, &err);

        if (resBook != SQLITE_OK || resAuthor != SQLITE_OK || 
            resMember != SQLITE_OK || resBorrowed != SQLITE_OK) {
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

    bool addBook(const string& title, const string& author, 
                const string& publicationDate, 
                 int availableCopies) {
        sqlite3_stmt* stmt;
        const char* sql = "INSERT INTO BOOK (TITLE, AUTHOR, publicationDATE, availableCopies) "
                          "VALUES (?, ?, ?, ?);";
        
        int rc = sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);
        if (rc != SQLITE_OK) {
            cout << "Error preparing statement: " << sqlite3_errmsg(db) << endl;
            return false;
        }

        // Bind values to prepared statement
        sqlite3_bind_text(stmt, 1, title.c_str(), -1, SQLITE_STATIC);
        sqlite3_bind_text(stmt, 2, author.c_str(), -1, SQLITE_STATIC);
        sqlite3_bind_text(stmt, 4, publicationDate.c_str(), -1, SQLITE_STATIC);
        sqlite3_bind_int(stmt, 5, availableCopies);

        // Execute the statement
        rc = sqlite3_step(stmt);
        if (rc != SQLITE_DONE) {
            cout << "Error inserting book: " << sqlite3_errmsg(db) << endl;
            sqlite3_finalize(stmt);
            return false;
        }

        sqlite3_finalize(stmt);
        cout << "Book added successfully." << endl;
        return true;
    }

    // Remove a book by ISBN (with prepared statement)
    bool removeBook(const string& isbn) {
        sqlite3_stmt* stmt;
        const char* sql = "DELETE FROM BOOK WHERE ID = ?;";
        
        int rc = sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);
        if (rc != SQLITE_OK) {
            cout << "Error preparing statement: " << sqlite3_errmsg(db) << endl;
            return false;
        }

        sqlite3_bind_text(stmt, 1, isbn.c_str(), -1, SQLITE_STATIC);

        rc = sqlite3_step(stmt);
        if (rc != SQLITE_DONE) {
            cout << "Error removing book: " << sqlite3_errmsg(db) << endl;
            sqlite3_finalize(stmt);
            return false;
        }

        sqlite3_finalize(stmt);
        cout << "Book removed successfully." << endl;
        return true;
    }

    // Borrow a book
    bool borrowBook(const string& memberName, const string& isbn) {
        sqlite3_stmt* stmt;
        int bookId = -1, memberId = -1;

        // Find book ID
        const char* findBookSql = "SELECT ID, availableCopies FROM BOOK WHERE ID = ?;";
        sqlite3_stmt* findBookStmt;
        int rc = sqlite3_prepare_v2(db, findBookSql, -1, &findBookStmt, NULL);
        sqlite3_bind_text(findBookStmt, 1, isbn.c_str(), -1, SQLITE_STATIC);
        
        if (sqlite3_step(findBookStmt) == SQLITE_ROW) {
            bookId = sqlite3_column_int(findBookStmt, 0);
            int availableCopies = sqlite3_column_int(findBookStmt, 1);
            
            if (availableCopies <= 0) {
                cout << "No copies available for this book." << endl;
                sqlite3_finalize(findBookStmt);
                return false;
            }
            sqlite3_finalize(findBookStmt);
        } else {
            cout << "Book not found." << endl;
            return false;
        }

        // Find or create member
        const char* findMemberSql = "SELECT ID FROM MEMBER WHERE NAME = ?;";
        sqlite3_stmt* findMemberStmt;
        rc = sqlite3_prepare_v2(db, findMemberSql, -1, &findMemberStmt, NULL);
        sqlite3_bind_text(findMemberStmt, 1, memberName.c_str(), -1, SQLITE_STATIC);
        
        if (sqlite3_step(findMemberStmt) != SQLITE_ROW) {
            // Member doesn't exist, create them
            sqlite3_finalize(findMemberStmt);
            const char* insertMemberSql = "INSERT INTO MEMBER (NAME) VALUES (?);";
            sqlite3_stmt* insertMemberStmt;
            rc = sqlite3_prepare_v2(db, insertMemberSql, -1, &insertMemberStmt, NULL);
            sqlite3_bind_text(insertMemberStmt, 1, memberName.c_str(), -1, SQLITE_STATIC);
            
            if (sqlite3_step(insertMemberStmt) != SQLITE_DONE) {
                cout << "Error adding member: " << sqlite3_errmsg(db) << endl;
                sqlite3_finalize(insertMemberStmt);
                return false;
            }
            memberId = sqlite3_last_insert_rowid(db);
            sqlite3_finalize(insertMemberStmt);
        } else {
            // Member exists
            memberId = sqlite3_column_int(findMemberStmt, 0);
            sqlite3_finalize(findMemberStmt);
        }

        // Record borrow transaction
        const char* borrowSql = "INSERT INTO BORROW (BOOK_ID, MEMBER_ID, BORROW_DATE) VALUES (?, ?, date('now'));";
        sqlite3_stmt* borrowStmt;
        rc = sqlite3_prepare_v2(db, borrowSql, -1, &borrowStmt, NULL);
        sqlite3_bind_int(borrowStmt, 1, bookId);
        sqlite3_bind_int(borrowStmt, 2, memberId);

        if (sqlite3_step(borrowStmt) != SQLITE_DONE) {
            cout << "Error recording borrow transaction: " << sqlite3_errmsg(db) << endl;
            sqlite3_finalize(borrowStmt);
            return false;
        }
        sqlite3_finalize(borrowStmt);

        // Update available copies
        const char* updateCopiesSql = "UPDATE BOOK SET availableCopies = availableCopies - 1 WHERE ID = ?;";
        sqlite3_stmt* updateStmt;
        rc = sqlite3_prepare_v2(db, updateCopiesSql, -1, &updateStmt, NULL);
        sqlite3_bind_int(updateStmt, 1, bookId);

        if (sqlite3_step(updateStmt) != SQLITE_DONE) {
            cout << "Error updating book copies: " << sqlite3_errmsg(db) << endl;
            sqlite3_finalize(updateStmt);
            return false;
        }
        sqlite3_finalize(updateStmt);

        cout << "Book borrowed successfully." << endl;
        return true;
    }

    // Return a book
    bool returnBook(const string& memberName, const string& isbn) {
        sqlite3_stmt* stmt;
        int bookId = -1, memberId = -1;

        // Find book and member IDs
        const char* findIdsSql = 
            "SELECT b.ID AS BOOK_ID, m.ID AS MEMBER_ID "
            "FROM BOOK b "
            "JOIN BORROW br ON b.ID = br.BOOK_ID "
            "JOIN MEMBER m ON m.ID = br.MEMBER_ID "
            "WHERE b.ISBN = ? AND m.NAME = ? AND br.RETURN_DATE IS NULL;";
        
        sqlite3_stmt* findIdsStmt;
        int rc = sqlite3_prepare_v2(db, findIdsSql, -1, &findIdsStmt, NULL);
        sqlite3_bind_text(findIdsStmt, 1, isbn.c_str(), -1, SQLITE_STATIC);
        sqlite3_bind_text(findIdsStmt, 2, memberName.c_str(), -1, SQLITE_STATIC);
        
        if (sqlite3_step(findIdsStmt) == SQLITE_ROW) {
            bookId = sqlite3_column_int(findIdsStmt, 0);
            memberId = sqlite3_column_int(findIdsStmt, 1);
            sqlite3_finalize(findIdsStmt);
        } else {
            cout << "No active borrow found for this book and member." << endl;
            return false;
        }

        // Update borrow record with return date
        const char* returnSql = "UPDATE BORROW SET RETURN_DATE = date('now') "
                                "WHERE BOOK_ID = ? AND MEMBER_ID = ? AND RETURN_DATE IS NULL;";
        sqlite3_stmt* returnStmt;
        rc = sqlite3_prepare_v2(db, returnSql, -1, &returnStmt, NULL);
        sqlite3_bind_int(returnStmt, 1, bookId);
        sqlite3_bind_int(returnStmt, 2, memberId);

        if (sqlite3_step(returnStmt) != SQLITE_DONE) {
            cout << "Error recording return transaction: " << sqlite3_errmsg(db) << endl;
            sqlite3_finalize(returnStmt);
            return false;
        }
        sqlite3_finalize(returnStmt);

        // Update available copies
        const char* updateCopiesSql = "UPDATE BOOK SET availableCopies = availableCopies + 1 WHERE ID = ?;";
        sqlite3_stmt* updateStmt;
        rc = sqlite3_prepare_v2(db, updateCopiesSql, -1, &updateStmt, NULL);
        sqlite3_bind_int(updateStmt, 1, bookId);

        if (sqlite3_step(updateStmt) != SQLITE_DONE) {
            cout << "Error updating book copies: " << sqlite3_errmsg(db) << endl;
            sqlite3_finalize(updateStmt);
            return false;
        }
        sqlite3_finalize(updateStmt);

        cout << "Book returned successfully." << endl;
        return true;
    }

    // Remove a member
    bool removeMember(const string& memberName) {
        sqlite3_stmt* stmt;
        const char* sql = "DELETE FROM MEMBER WHERE ID = ?;";
        
        int rc = sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);
        if (rc != SQLITE_OK) {
            cout << "Error preparing statement: " << sqlite3_errmsg(db) << endl;
            return false;
        }

        sqlite3_bind_text(stmt, 1, memberName.c_str(), -1, SQLITE_STATIC);

        rc = sqlite3_step(stmt);
        if (rc != SQLITE_DONE) {
            cout << "Error removing member: " << sqlite3_errmsg(db) << endl;
            sqlite3_finalize(stmt);
            return false;
        }

        sqlite3_finalize(stmt);
        cout << "Member removed successfully." << endl;
        return true;
    }

    // List all books (with prepared statement)
    void listBooks() {
        sqlite3_stmt* stmt;
        const char* sql = "SELECT * FROM BOOK;";
        
        int rc = sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);
        if (rc != SQLITE_OK) {
            cout << "Error preparing statement: " << sqlite3_errmsg(db) << endl;
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

        // Fetch and print results
        while ((rc = sqlite3_step(stmt)) == SQLITE_ROW) {
            for (size_t i = 0; i < headers.size(); ++i) {
                const char* value = reinterpret_cast<const char*>(sqlite3_column_text(stmt, i));
                cout << left << setw(20) << (value ? value : "NULL");
            }
            cout << endl;
        }

        if (rc != SQLITE_DONE) {
            cout << "Error listing books: " << sqlite3_errmsg(db) << endl;
        }

        sqlite3_finalize(stmt);
        cout << string(125, '-') << endl;
    }

    // Add an author (simplified)
    bool addAuthor(const string& name, const string& books = "") {
        sqlite3_stmt* stmt;
        const char* sql = "INSERT INTO AUTHOR (NAME, BOOKS) VALUES (?,?);";
        
        int rc = sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);
        if (rc != SQLITE_OK) {
            cout << "Error preparing statement: " << sqlite3_errmsg(db) << endl;
            return false;
        }

        sqlite3_bind_text(stmt, 1, name.c_str(), -1, SQLITE_STATIC);

        rc = sqlite3_step(stmt);
        if (rc != SQLITE_DONE) {
            cout << "Error adding author: " << sqlite3_errmsg(db) << endl;
            sqlite3_finalize(stmt);
            return false;
        }

        sqlite3_finalize(stmt);
        cout << "Author added successfully." << endl;
        return true;
    }

    // List all authors
    void listAuthors() {
        sqlite3_stmt* stmt;
        const char* sql = "SELECT * FROM AUTHOR;";
        
        int rc = sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);
        if (rc != SQLITE_OK) {
            cout << "Error preparing statement: " << sqlite3_errmsg(db) << endl;
            return;
        }

        // Author headers
        vector<string> headers;
        headers.push_back("ID");
        headers.push_back("Name");
        headers.push_back("Books");
        
        printTableHeader(headers);

        // Fetch and print results
        while ((rc = sqlite3_step(stmt)) == SQLITE_ROW) {
            for (size_t i = 0; i < headers.size(); ++i) {
                const char* value = reinterpret_cast<const char*>(sqlite3_column_text(stmt, i));
                cout << left << setw(20) << (value ? value : "NULL");
            }
            cout << endl;
        }

        if (rc != SQLITE_DONE) {
            cout << "Error listing authors: " << sqlite3_errmsg(db) << endl;
        }

        sqlite3_finalize(stmt);
        cout << string(125, '-') << endl;
    }

    // Add a book for a specific author
    bool addBookForAuthor(const string& authorName, const string& title, 
                          const string& isbn, const string& publicationDate, 
                          int availableCopies) {
        // First, check if the author exists
        sqlite3_stmt* stmt;
        const char* checkAuthorSql = "SELECT ID FROM AUTHOR WHERE NAME = ?;";
        
        int rc = sqlite3_prepare_v2(db, checkAuthorSql, -1, &stmt, NULL);
        if (rc != SQLITE_OK) {
            cout << "Error preparing statement: " << sqlite3_errmsg(db) << endl;
            return false;
        }

        sqlite3_bind_text(stmt, 1, authorName.c_str(), -1, SQLITE_STATIC);

        rc = sqlite3_step(stmt);
        sqlite3_finalize(stmt);

        if (rc != SQLITE_ROW) {
            cout << "Author not found. Please add the author first." << endl;
            return false;
        }

        // If author exists, add the book
        return addBook(title, authorName, publicationDate, availableCopies);
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
                    cout << "4. Borrow Book\n";
                    cout << "5. Return Book\n";
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
                            cout << "Enter Publication Date: ";
                            getline(cin, publicationDate);
                            cout << "Enter Available Copies: ";
                            cin >> availableCopies;
                            addBook(title, author, publicationDate, availableCopies);
                            break;
                        case 2:
                            cout << "Enter ID to remove: ";
                            cin >> isbn;
                            removeBook(isbn);
                            break;
                        case 3:
                            listBooks();
                            break;
                        case 4: {
                            string memberID, isbn;
                            cout << "Enter Member ID: ";
                            cin.ignore(numeric_limits<streamsize>::max(), '\n'); // Clear the input buffer
                            getline(cin, memberID);
                            cout << "Enter ID of book to borrow: ";
                            getline(cin, isbn);
                            borrowBook(memberID, isbn);
                            break;
                        }
                        case 5: {
                            string memberID, isbn;
                            cout << "Enter Member ID: ";
                            cin.ignore(numeric_limits<streamsize>::max(), '\n'); // Clear the input buffer
                            getline(cin, memberID);
                            cout << "Enter ID of book to return: ";
                            getline(cin, isbn);
                            returnBook(memberID, isbn);
                            break;
                        }
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
                             cin.ignore(numeric_limits<streamsize>::max(), '\n'); // Clear the input buffer
                            getline(cin, name);
                            cout << "Enter Author Books:";
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
                    cout << "3. Remove Member\n";
                    cout << "Enter your choice: ";
                    cin >> memberChoice;

                    string name, contactInfo;

                    switch (memberChoice) {
                        case 1:
                            cout << "Enter Member Name: ";
                            cin.ignore(numeric_limits<streamsize>::max(), '\n'); // Clear the input buffer
                            getline(cin, name);
                            cout << "Enter Contact Info (optional): ";
                            getline(cin, contactInfo);
                            addMember(name, contactInfo);
                            break;
                        case 2:
                            listMembers();
                            break;
                        case 3: {
                            string id;
                            cout << "Enter Member ID to remove: ";
                            cin.ignore(numeric_limits<streamsize>::max(), '\n'); // Clear the input buffer
                            getline(cin, id);
                            removeMember(id);
                            break;
                        }
                        default:
                            cout << "Invalid choice\n";
                            break;
                    }
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