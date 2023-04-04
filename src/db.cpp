#include "db.hpp"

#include <iostream>

namespace mqr {
db::db() : dbp(nullptr) {}

int db::open(const char* path) {
    if (dbp) {
        std::cout << __FUNCTION__ << ": another db is currently open " << std::endl;
        return -1;
    }

    if (sqlite3_open(path, &dbp) != SQLITE_OK) {
        std::cout << __FUNCTION__ << ": failed open " << path << std::endl;
        return -2;
    }

    const std::string query = "CREATE TABLE songs(id INT NOT NULL UNIQUE,PRIMARY KEY(id));";
    sqlite3_exec(dbp, query.c_str(), nullptr, nullptr, nullptr);

    return 0;
}

int db::transaction_begin() {
    if (!dbp) return -1;

    const std::string query = "BEGIN TRANSACTION;";
    char* errmsg = nullptr;

    int status = 0;
    if (sqlite3_exec(dbp, query.c_str(), nullptr, nullptr, &errmsg) != SQLITE_OK) {
        std::cout << __FUNCTION__ << ": failed " << query << std::endl;
        std::cout << __FUNCTION__ << ": " << errmsg << std::endl;

        sqlite3_free(errmsg);
        status = -2;
    }

    return status;
}

int db::insert(const uint32_t id) {
    if (!dbp) return -1;

    const std::string sql_insert = "INSERT INTO songs(id) VALUES(";
    const std::string query = sql_insert + std::to_string(id) + ");";
    char* errmsg = nullptr;

    int status = 0;
    if (sqlite3_exec(dbp, query.c_str(), nullptr, nullptr, &errmsg) != SQLITE_OK) {
        std::cout << __FUNCTION__ << ": failed " << query << std::endl;
        std::cout << __FUNCTION__ << ": " << errmsg << std::endl;

        sqlite3_free(errmsg);
        status = -2;
    }

    return status;
}

int db::transaction_end() {
    if (!dbp) return -1;

    const std::string query = "END TRANSACTION;";
    char* errmsg = nullptr;

    int status = 0;
    if (sqlite3_exec(dbp, query.c_str(), nullptr, nullptr, &errmsg) != SQLITE_OK) {
        std::cout << __FUNCTION__ << ": failed " << query << std::endl;
        std::cout << __FUNCTION__ << ": " << errmsg << std::endl;

        sqlite3_free(errmsg);
        status = -2;
    }

    return status;
}

static int id_exists_callback(void* exists, int count, char** data, char** columns) {
    /* *exists = true  if there is 1 column
     * *exists = false if there is no columns */

    bool* exists_ptr = static_cast<bool*>(exists);
    *exists_ptr = count;

    return 0;
}

bool db::id_exists(const uint32_t id) {
    if (!dbp) return false;

    const std::string sql_exists = "SELECT 1 FROM songs WHERE id=";
    const std::string query = sql_exists + std::to_string(id) + ";";
    char* errmsg = nullptr;

    bool exists = false;
    if (sqlite3_exec(dbp, query.c_str(), id_exists_callback, (void*)&exists, &errmsg) != SQLITE_OK) {
        std::cout << __FUNCTION__ << ": failed " << query << std::endl;
        std::cout << __FUNCTION__ << ": " << errmsg << std::endl;

        sqlite3_free(errmsg);
    }

    return exists;
}

int db::close() {
    if (dbp) {
        sqlite3_close(dbp);
        dbp = nullptr;
    }

    return 0;
}
}  // namespace mqr
