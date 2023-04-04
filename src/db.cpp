#include "db.hpp"

#include <iostream>

// NOTE: Queries must end with ;

bool db_open(const char* path, sqlite3** dbpp) {
    if (sqlite3_open(path, dbpp) != SQLITE_OK) {
        std::cout << __FUNCTION__ << ": failed open " << path << std::endl;
        return false;
    }

    constexpr char sql_create[] = "CREATE TABLE songs(id INT NOT NULL UNIQUE,PRIMARY KEY(id));";
    sqlite3_exec(*dbpp, sql_create, nullptr, nullptr, nullptr);

    return true;
}

void db_begin(sqlite3* dbp) {
    if (!dbp) return;

    std::string query = "BEGIN TRANSACTION;";
    char* errmsg = nullptr;

    if (sqlite3_exec(dbp, query.c_str(), nullptr, nullptr, &errmsg) != SQLITE_OK) {
        std::cout << __FUNCTION__ << ": failed " << query << std::endl;
        std::cout << __FUNCTION__ << ": " << errmsg << std::endl;

        sqlite3_free(errmsg);
    }
}

void db_insert(sqlite3* dbp, const uint32_t id) {
    if (!dbp) return;

    std::string sql_insert = "INSERT INTO songs(id) VALUES(";
    std::string query = sql_insert + std::to_string(id) + ");";
    char* errmsg = nullptr;

    if (sqlite3_exec(dbp, query.c_str(), nullptr, nullptr, &errmsg) != SQLITE_OK) {
        std::cout << __FUNCTION__ << ": failed " << query << std::endl;
        std::cout << __FUNCTION__ << ": " << errmsg << std::endl;

        sqlite3_free(errmsg);
    }
}

void db_end(sqlite3* dbp) {
    if (!dbp) return;

    std::string query = "END TRANSACTION;";
    char* errmsg = nullptr;

    if (sqlite3_exec(dbp, query.c_str(), nullptr, nullptr, &errmsg) != SQLITE_OK) {
        std::cout << __FUNCTION__ << ": failed " << query << std::endl;
        std::cout << __FUNCTION__ << ": " << errmsg << std::endl;

        sqlite3_free(errmsg);
    }
}

static int db_exists_callback(void* exists, int count, char** data, char** columns) {
    /* *exists = true  if there is 1 column
     * *exists = false if there is no columns */

    bool* exists_ptr = static_cast<bool*>(exists);
    *exists_ptr = count;

    return 0;
}

bool db_id_exists(const uint32_t id, sqlite3* dbp) {
    if (!dbp) return false;

    std::string sql_exists = "SELECT 1 FROM songs WHERE id=";
    std::string query = sql_exists + std::to_string(id) + ";";
    char* errmsg = nullptr;

    bool exists = false;
    if (sqlite3_exec(dbp, query.c_str(), db_exists_callback, (void*)&exists, &errmsg) != SQLITE_OK) {
        std::cout << __FUNCTION__ << ": failed " << query << std::endl;
        std::cout << __FUNCTION__ << ": " << errmsg << std::endl;

        sqlite3_free(errmsg);
    }

    return exists;
}

void db_close(sqlite3* dbp) {
    if (dbp) sqlite3_close(dbp);
}
