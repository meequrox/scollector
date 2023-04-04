#ifndef DB_HPP
#define DB_HPP

#include <sqlite3.h>

#include <cstdint>

namespace mqr {
class db {
   private:
    /* dbp values:
     * nullptr if db not opened
     * 0xADDR if db opened */
    sqlite3* dbp;

   public:
    db();

    // NOTE: SQL queries must end with ;
    int open(const char* path);

    int transaction_begin();
    int insert(const uint32_t id);
    int transaction_end();

    bool id_exists(const uint32_t id);
    int close();
};
}  // namespace mqr

#endif  // DB_HPP
