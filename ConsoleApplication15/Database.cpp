#include "Database.h"

// конструктор
Database::Database(const std::string& db_name) {
    int exit = sqlite3_open(db_name.c_str(), &db);
    if (exit) {
        std::cerr << "Ошибка открытия БД: " << sqlite3_errmsg(db) << std::endl;
    }
}

//деструктор
Database::~Database() {
    sqlite3_close(db);
}

// sql запросфы
bool Database::execute(const std::string& sql) {
    char* errmsg;
    int exit = sqlite3_exec(db, sql.c_str(), nullptr, 0, &errmsg);
    if (exit != SQLITE_OK) {
        std::cerr << "Ошибка выполнения SQL: " << errmsg << std::endl;
        sqlite3_free(errmsg);
        return false;
    }
    return true;
}
