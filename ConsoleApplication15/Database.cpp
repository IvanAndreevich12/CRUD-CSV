#include "Database.h"

// �����������
Database::Database(const std::string& db_name) {
    int exit = sqlite3_open(db_name.c_str(), &db);
    if (exit) {
        std::cerr << "������ �������� ��: " << sqlite3_errmsg(db) << std::endl;
    }
}

//����������
Database::~Database() {
    sqlite3_close(db);
}

// sql ��������
bool Database::execute(const std::string& sql) {
    char* errmsg;
    int exit = sqlite3_exec(db, sql.c_str(), nullptr, 0, &errmsg);
    if (exit != SQLITE_OK) {
        std::cerr << "������ ���������� SQL: " << errmsg << std::endl;
        sqlite3_free(errmsg);
        return false;
    }
    return true;
}
