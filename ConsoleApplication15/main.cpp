
#include <iostream>
#include "Database.h"
#include <fstream>
#include <sstream>
#include <vector>
#include <Windows.h>
using namespace std;

// выполнение CRUD операций
class CrudOperations {
public:
    CrudOperations(Database& database) : db(database) {}


    // создаем табилцу стуенды
    void createTable() {
        string sql = "CREATE TABLE IF NOT EXISTS students("
            "id INTEGER PRIMARY KEY AUTOINCREMENT, "
            "name TEXT NOT NULL, "
            "age INTEGER NOT NULL);";

        db.execute(sql);
    }

    // добавление новых данных в таблицу, автоинкрементация идёт дальше
    void insertData(const string& name, int age) {
        string sql = "INSERT INTO students (name, age) VALUES ('" + name + "', " + to_string(age) + ");";
       
        db.execute(sql);
    }

    // все столбцы и строки
    void readData() {
        string sql = "SELECT * FROM students;";

        sqlite3_stmt* stmt;

        sqlite3_prepare_v2(db.getDb(), sql.c_str(), -1, &stmt, nullptr);

        while (sqlite3_step(stmt) == SQLITE_ROW) {
            cout << "ID: " << sqlite3_column_int(stmt, 0)
                << " Name: " << sqlite3_column_text(stmt, 1)
                << " Age: " << sqlite3_column_int(stmt, 2) <<endl;
        }

        sqlite3_finalize(stmt);
    }

    // обновляем уже существующие строки по ID
    void updateData(int id, const string& name, int age) {
        string sql = "UPDATE students SET name = '" + name + "', age = " + to_string(age) +
            " WHERE id = " + to_string(id) + ";";
        
        db.execute(sql);
    }

    // удаляем выбранные строки по ID
    void deleteData(int id) {
        string sql = "DELETE FROM students WHERE id = " + to_string(id) + ";";

        db.execute(sql);
    }

    // чистим всю таблицу
    void clearTable() {
        string sql = "DELETE FROM students;";

        db.execute(sql);
    }

    //удаляем таблицу, прийдётся перезапустить программу, так как нету отдельной команды на создание новой таблицы, если требуется можно сделать
    void dropTable() {
        string sql = "DROP TABLE IF EXISTS students;";

        db.execute(sql);
    }

private:
    Database& db;
};



// импор данных из CSV
class CsvImporter {
public:
    CsvImporter(Database& database) : db(database) {}
    

    //читаем файл CSV который находится у нас в папке с остальными всеми файлами
    void importCsv(const string& filename) {
        ifstream file(filename);

        //еЕсли не нашли его
        if (!file.is_open()) {
            cout << "Ошибка открытия файла" << endl;
            return;
        }
        

        //еашли его
        string line;
        while (getline(file, line)) {

            //созданный файл имеет структуру хранения данных (имя студента, 20 (возраст студента)) делим через запятую данные
            vector<string> values = split(line, ',');

            //если в строке больше 2 данных то добавляем, значит у нас будут данные чтобы добавить всю строку
            if (values.size() == 2) {

                string name = values[0];
                int age = stoi(values[1]);

                string sql = "INSERT INTO students (name, age) VALUES ('" + name + "', " + to_string(age) + ");";

                db.execute(sql);
            }
        }

        file.close();
    }

private:
    Database& db;

    vector<string> split(const string& s, char delimiter) {
        vector<string> tokens;
        string token;
        istringstream tokenStream(s);

        while (getline(tokenStream, token, delimiter)) {
            tokens.push_back(token);
        }
        return tokens;
    }
};

int main() {

    //докальность делаем русской
    setlocale(LC_ALL, "ru");

    Database db("students.db"); // создание базы данных
    CrudOperations crud(db); // создание объекта для операций CRUD
    CsvImporter csvImporter(db); // создание объекта для импорта CSV

    crud.createTable(); // создание таблицы, отдельной команды нету, только при открытие создается, если потребуется можно добавить

    // принимаем команды и работаем с функциями и делаем SQL-запросы
    string command;
    while (true) {
        cout << "Введите команду (/add, /read, /update, /delete, /import, /clear, /drop, /exit): ";
        cin >> command;

        if (command == "/add") {
            crud.insertData("Charlize Theron", 20);
            crud.insertData("Melanie Laurent", 22);

            cout << "Тестовые данные добавлены.\n";
        }

        else if (command == "/read") {
            cout << "Чтение данных:\n";

            crud.readData();
        }

        else if (command == "/update") {
            crud.updateData(1, "Jackie Chan", 21);
            crud.updateData(2, "Penelope Cruz", 23);

            cout << "Данные обновлены.\n";
        }

        else if (command == "/delete") {
            crud.deleteData(2);

            cout << "Данные удалены.\n";
        }

        else if (command == "/import") {
            csvImporter.importCsv("students.csv");

            cout << "Данные импортированы из CSV.\n";
        }

        else if (command == "/clear") {
            crud.clearTable();

            cout << "Таблица очищена.\n";
        }

        else if (command == "/drop") {
            crud.dropTable();

            cout << "Таблица удалена.\n";
        }

        else if (command == "/exit") {

            break;
        }

        else {
            cout << "Неизвестная команда.\n";
        }
    }


    return 0;
}



// Запуск программы: CTRL+F5 или меню "Отладка" > "Запуск без отладки"
// Отладка программы: F5 или меню "Отладка" > "Запустить отладку"

// Советы по началу работы 
//   1. В окне обозревателя решений можно добавлять файлы и управлять ими.
//   2. В окне Team Explorer можно подключиться к системе управления версиями.
//   3. В окне "Выходные данные" можно просматривать выходные данные сборки и другие сообщения.
//   4. В окне "Список ошибок" можно просматривать ошибки.
//   5. Последовательно выберите пункты меню "Проект" > "Добавить новый элемент", чтобы создать файлы кода, или "Проект" > "Добавить существующий элемент", чтобы добавить в проект существующие файлы кода.
//   6. Чтобы снова открыть этот проект позже, выберите пункты меню "Файл" > "Открыть" > "Проект" и выберите SLN-файл.
