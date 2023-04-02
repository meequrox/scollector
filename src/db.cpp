#include "db.hpp"

#include <iostream>

// NOTE: Queries must end with ;

bool db_start(const char* path, sqlite3** dbpp) {
    if (sqlite3_open(path, dbpp) != SQLITE_OK) {
        std::cout << __FUNCTION__ << ": failed open " << path << std::endl;
        return false;
    }

    constexpr char sql_create[] = "CREATE TABLE songs(id INT NOT NULL,PRIMARY KEY(id));";
    sqlite3_exec(*dbpp, sql_create, nullptr, nullptr, nullptr);
    return true;
}

void db_end(sqlite3* dbp) {
    if (dbp) sqlite3_close(dbp);
}

void db_insert(sqlite3* dbp, uint32_t id) {
    if (!dbp) return;

    constexpr char sql_insert[] = "INSERT INTO songs(id) VALUES";
    std::string query = std::string(sql_insert) + "(" + std::to_string(id) + ");";

    if (sqlite3_exec(dbp, query.c_str(), nullptr, nullptr, nullptr) != SQLITE_OK) {
        std::cout << __FUNCTION__ << ": failed " << query << std::endl;
    }
}

int db_exists_callback(void* exists, int count, char** data, char** columns) {
    /* *exists = true  if there is 1 column
     * *exists = false if there is no columns */

    bool* exists_ptr = static_cast<bool*>(exists);
    *exists_ptr = count;

    return 0;
}

bool db_id_exists(uint32_t id, sqlite3* dbp) {
    if (!dbp) return false;

    constexpr char sql_exists[] = "SELECT 1 FROM songs WHERE id=";
    std::string query = std::string(sql_exists) + std::to_string(id) + ";";

    bool exists = false;
    if (sqlite3_exec(dbp, query.c_str(), db_exists_callback, (void*)&exists, nullptr) != SQLITE_OK) {
        std::cout << __FUNCTION__ << ": failed " << query << std::endl;
    }

    return exists;
}
