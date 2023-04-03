#ifndef DB_HPP
#define DB_HPP

#include <sqlite3.h>

#include <cstdint>

bool db_open(const char* path, sqlite3** dbpp);

void db_begin(sqlite3* dbp);
void db_insert(sqlite3* dbp, const uint32_t id);
void db_end(sqlite3* dbp);

bool db_id_exists(const uint32_t id, sqlite3* dbp);

void db_close(sqlite3* dbp);

#endif  // DB_HPP
