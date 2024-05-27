#include <iostream>
#include <sqlite3.h>

static int callback(void *NotUsed, int argc, char **argv, char **azColName) {
    for(int i = 0; i < argc; i++) {
        std::cout << azColName[i] << ": " << (argv[i] ? argv[i] : "NULL") << "<br>";
    }
    std::cout << "<br>";
    return 0;
}

int main() {
    sqlite3 *db;
    char *zErrMsg = 0;
    int rc;

    rc = sqlite3_open("test.db", &db);

    if(rc) {
        std::cout << "Content-type: text/html\n\n";
        std::cout << "Can't open database: " << sqlite3_errmsg(db) << "<br>";
        return 0;
    } else {
        const char *sql = "SELECT * FROM users;";
        std::cout << "Content-type: text/html\n\n";
        rc = sqlite3_exec(db, sql, callback, 0, &zErrMsg);

        if(rc != SQLITE_OK) {
            std::cout << "SQL error: " << zErrMsg << "<br>";
            sqlite3_free(zErrMsg);
        }
    }
    sqlite3_close(db);
    return 0;
}