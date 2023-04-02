#ifndef DB_HPP
#define DB_HPP

#include <sqlite3.h>

#include <cstdint>

bool db_start(const char* path, sqlite3** dbpp);
void db_insert(sqlite3* dbp, uint32_t id);
bool db_id_exists(uint32_t id, sqlite3* dbp);
void db_end(sqlite3* dbp);

#endif  // DB_HPP
