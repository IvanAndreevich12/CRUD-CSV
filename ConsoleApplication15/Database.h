#ifndef DATABASE_H
#define DATABASE_H

#include <sqlite3.h>
#include <iostream>
#include <string>

// работа с бд
class Database {
public:
    Database(const std::string& db_name);
    ~Database();
    bool execute(const std::string& sql);
    sqlite3* getDb() { return db; }

private:
    sqlite3* db;
};

#endif // DATABASE_H
