
#include <iostream>
#include "Database.h"
#include <fstream>
#include <sstream>
#include <vector>
#include <Windows.h>
#include <regex> //  библиотека для регулярных выражений

using namespace std;

// выполнение CRUD операций
class CrudOperations {
public:
    CrudOperations(Database& database) : db(database) {}


    // создаем табилцу стуенды
    void createTable() {
        //тут я меняю тип данных  на TEXT чтобы дату хранить
        string sql = "CREATE TABLE IF NOT EXISTS students("
            "id INTEGER PRIMARY KEY AUTOINCREMENT, "
            "name TEXT NOT NULL, "
            "birthdate TEXT NOT NULL);";

        db.execute(sql);
    }

    // добавление новых данных в таблицу, автоинкрементация идёт дальше
    //теперь принимаем дату как строку
    void insertData(const string& name, const string& birthdate) {
        string sql = "INSERT INTO students (name, birthdate) VALUES ('" + name + "', '" + birthdate + "');";

        db.execute(sql);
    }

    // все столбцы и строки
    void readData() {
        string sql = "SELECT * FROM students;";

        sqlite3_stmt* stmt;

        sqlite3_prepare_v2(db.getDb(), sql.c_str(), -1, &stmt, nullptr);


        while (sqlite3_step(stmt) == SQLITE_ROW) {
            cout << "ID: " << sqlite3_column_int(stmt, 0)
                << " Имя: " << sqlite3_column_text(stmt, 1)
                << " Дата рождения: " << sqlite3_column_text(stmt, 2) << endl; //и тут выводим дату рождения
        }

        sqlite3_finalize(stmt);
    }

    // обновляем уже существующие строки по ID
    //и тут меняем на строку для даты
    void updateData(int id, const string& name, const string& birthdate) {
        string sql = "UPDATE students SET name = '" + name + "', birthdate = '" + birthdate +
            "' WHERE id = " + to_string(id) + ";";

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

            //если в строке 2 данных то добавляем, значит у нас будут данные чтобы добавить всю строку
            if (values.size() == 2) {

                string name = values[0];
                string birthdate = values[1]; //теперь у нас не возраст, а дата рождения!!

                string sql = "INSERT INTO students (name, birthdate) VALUES ('" + name + "', '" + birthdate + "');";

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


// проверка формата и минимальной реальности даты
bool isValidDate(const string& date) {
    //  регулярное выражение для проверки формата YYYY-MM-DD
    regex pattern("^(\\d{4})-(\\d{2})-(\\d{2})$");
    // открываем строку ^
    // (первый ящик и 4 символа)-(2 символа)-(2символа)   (дефис это дефис)
    // $ - закрываем


    if (!regex_match(date, pattern)) {
        return false; // Неверный формат
    }

    // год, месяц и день
    int year, month, day;
    sscanf_s(date.c_str(), "%d-%d-%d", &year, &month, &day);

    // проверка минимального года
    if (year < 1900) {
        return false; // слишком ранний год
    }

    // сверка количества месяцев
    if (month < 1 || month > 12) {
        return false; // некорректен
    }

    // смотрим количества дней
    if (day < 1 || day > 31) {
        return false; // некорректен
    }

    return true; // валид
}



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
            string name, birthdate;
            cout << "Введите имя студента: ";
            cin.ignore();
            getline(cin, name);

            // Цикл ввода даты с постоянной проверкой
            while (true) {
                cout << "Введите дату рождения (YYYY-MM-DD): ";
                cin >> birthdate;

                if (isValidDate(birthdate)) {
                    crud.insertData(name, birthdate);
                    cout << "Данные добавлены.\n";
                    break;
                }
                else {
                    cout << "Неверный формат даты. Пожалуйста, введите дату в формате YYYY-MM-DD.\n";
                }
            }
        }


        else if (command == "/read") {
            cout << "Чтение данных:\n";

            crud.readData();
        }


        else if (command == "/update") {
            int id;
            string name, birthdate; //birthdate строка
            cout << "Введите ID студента для обновления: ";
            cin >> id;
            cout << "Введите новое имя: ";
            cin.ignore(); //ЧИстим буфер перед getline
            getline(cin, name);

            // кикл ввода даты с проверкой
            while (true) {
                cout << "Введите дату рождения (YYYY-MM-DD): ";
                cin >> birthdate;

                if (isValidDate(birthdate)) {
                    crud.updateData(id, name, birthdate); //обновляем, передавая дату как строку
                    // не делаем проверку каждый ра0 при вводе id, если нету такого id то просто ничего не обновиться

                    cout << "Данные обновлены.\n";
                    break;
                }
                else {
                    cout << "Неверный формат даты. Пожалуйста, введите дату в формате YYYY-MM-DD.\n";
                }
            }
        }


        else if (command == "/delete") {
            int id;
            cout << "Введите ID студента для удаления: ";
            cin >> id;
            crud.deleteData(id);

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
