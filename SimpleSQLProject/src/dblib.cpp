#include "dblib.h"

namespace dblib {
    int Database::selectCallback(void* data, int argc, char** argv, char** azColName) {
        std::string& output = *static_cast<std::string*>(data);
        for (int i = 0; i < argc; i++) {
            output += azColName[i];
            output += " = ";
            output += (argv[i] ? argv[i] : "NULL");
            output += "\n";
        }
        return 0;
    }

    void Database::handleSQLiteError(sqlite3* db, const std::string& message) const {
        std::cout << "[ERROR] " << message << ": " << sqlite3_errmsg(db);
        sqlite3_close(db);
    }

    bool Database::executeSQL(const std::string& sql) {
        int rc = sqlite3_open(path.c_str(), &db);
        if (rc != SQLITE_OK) {
            handleSQLiteError(db, "Failed to connect to database");
            return false;
        }

        rc = sqlite3_exec(db, sql.c_str(), NULL, 0, NULL);
        if (rc != SQLITE_OK) {
            handleSQLiteError(db, "Failed to execute SQL code");
            return false;
        }

        sqlite3_close(db);
        return true;
    }

    Database::Database(const std::string& filePath) : path(filePath) {
        int rc = sqlite3_open(path.c_str(), &db);
        if (rc != SQLITE_OK)
            handleSQLiteError(db, "Failed to create database");

        sqlite3_close(db);
    }

    void Database::createTable(const std::string& table, const std::string& tableDefinition) {
        const std::string sql = "CREATE TABLE IF NOT EXISTS " + table + "(" + tableDefinition + ");";
        executeSQL(sql);
    }

    void Database::insertData(const std::string& table, const std::string& columns, const std::string& values) {
        const std::string sql = "INSERT INTO " + table + " (" + columns + ") VALUES (" + values + ");";
        executeSQL(sql);
    }

    void Database::updateData(const std::string& table, const std::string& setValues, const std::string& condition) {
        const std::string sql = "UPDATE " + table + " SET " + setValues + " WHERE " + condition + ";";
        executeSQL(sql);
    }

    void Database::deleteData(const std::string& table, const std::string& condition) {
        const std::string sql = "DELETE FROM " + table + " WHERE " + condition + ";";
        executeSQL(sql);
    }

    std::string Database::selectData(const std::string& table, const std::string& columns, const std::string& condition) {
        std::string data;
        int rc = sqlite3_open(path.c_str(), &db);

        if (rc != SQLITE_OK) {
            handleSQLiteError(db, "Failed to connect to database");
            return "";
        }

        std::string sqlCode = "SELECT " + columns + " FROM " + table;
        if (!condition.empty()) {
            sqlCode += " WHERE " + condition;
        }
        sqlCode += ";";

        rc = sqlite3_exec(db, sqlCode.c_str(), selectCallback, &data, NULL);

        if (rc != SQLITE_OK) {
            handleSQLiteError(db, "Failed to execute SQL code");
            sqlite3_close(db);
            return "";
        }

        sqlite3_close(db);
        return data;
    }

    std::string Database::selectAllData(const std::string& table) {
        std::string data;
        int rc = sqlite3_open(path.c_str(), &db);

        if (rc != SQLITE_OK) {
            handleSQLiteError(db, "Failed to connect to database");
            return "";
        }

        const std::string sqlCode = "SELECT * FROM " + table + ";";
        rc = sqlite3_exec(db, sqlCode.c_str(), selectCallback, &data, NULL);

        if (rc != SQLITE_OK) {
            handleSQLiteError(db, "Failed to execute SQL code");
            sqlite3_close(db);
            return "";
        }

        sqlite3_close(db);
        return "";
    }
}

/*
namespace dblib {
    class Database {
    protected:
        sqlite3* db;
        std::string path;
    
        // Callback-Funktion für die Ausgabe von ausgewählten Daten
        static int selectCallback(void* data, int argc, char** argv, char** azColName) {
            std::string& output = *static_cast<std::string*>(data);
            for (int i = 0; i < argc; i++) {
                output += azColName[i];
                output += " = ";
                output += (argv[i] ? argv[i] : "NULL");
                output += "\n";
            }
            return 0;
        }
    
        void handleSQLiteError(sqlite3* db, const std::string& message) const {
            std::cout << "[ERROR] " << message << ": " << sqlite3_errmsg(db);
            sqlite3_close(db);
        }
    
        bool executeSQL(const std::string& sql) {
            int rc = sqlite3_open(path.c_str(), &db);
            if (rc != SQLITE_OK) {
                handleSQLiteError(db, "Failed to connect to database");
                return false;
            }
    
            rc = sqlite3_exec(db, sql.c_str(), NULL, 0, NULL);
            if (rc != SQLITE_OK) {
                handleSQLiteError(db, "Failed to execute SQL code");
                return false;
            }
    
            sqlite3_close(db);
            return true;
        }
    
    public:
        // Create a new database
        Database(const std::string& filePath) : path(filePath) {
            int rc = sqlite3_open(path.c_str(), &db);
            if (rc != SQLITE_OK)
                handleSQLiteError(db, "Failed to create database");
    
            sqlite3_close(db);
        }
    
        Database(const Database&) = delete;
        void operator=(const Database&) = delete;
    
        // SQL: CREATE TABLE IF NOT EXISTS {table} ({tableDefinition});
        void createTable(const std::string &table, const std::string &tableDefinition) {
            const std::string sql = "CREATE TABLE IF NOT EXISTS " + table + "(" + tableDefinition + ");";
            executeSQL(sql);
        }
    
        // SQL: INSERT INTO {table} ({columns}) VALUES ({values});
        void insertData(const std::string &table, const std::string & columns, const std::string &values) {
            const std::string sql = "INSERT INTO " + table + " (" + columns + ") VALUES (" + values + ");";
            executeSQL(sql);
        }
    
        // SQL: UPDATE {table} SET {setValues} WHERE {condition};
        void updateData(const std::string &table, const std::string &setValues, const std::string &condition) {
            const std::string sql = "UPDATE " + table + " SET " + setValues + " WHERE " + condition + ";";
            executeSQL(sql);
        }
    
        // SQL: DELETE FROM {table} WHERE {condition};
        void deleteData(const std::string &table, const std::string &condition) {
            const std::string sql = "DELETE FROM " + table + " WHERE " + condition + ";";
            executeSQL(sql);
        }
    
        // SQL: SELECT {columns} FROM {table} WHERE {condition};
        std::string selectData(const std::string& table, const std::string& columns, const std::string& condition) {
            std::string data;
            int rc = sqlite3_open(path.c_str(), &db);
    
            if (rc != SQLITE_OK) {
                handleSQLiteError(db, "Failed to connect to database");
                return "";
            }
    
            std::string sqlCode = "SELECT " + columns + " FROM " + table;
            if (!condition.empty()) {
                sqlCode += " WHERE " + condition;
            }
            sqlCode += ";";
    
            rc = sqlite3_exec(db, sqlCode.c_str(), selectCallback, &data, NULL);
    
            if (rc != SQLITE_OK) {
                handleSQLiteError(db, "Failed to execute SQL code");
                sqlite3_close(db);
                return "";
            }
    
            sqlite3_close(db);
            return data;
        }
    
        // SQL: SELECT * FROM {table};
        std::string selectAllData(const std::string& table) {
            std::string data;
            int rc = sqlite3_open(path.c_str(), &db);
    
            if (rc != SQLITE_OK) {
                handleSQLiteError(db, "Failed to connect to database");
                return "";
            }
    
            const std::string sqlCode = "SELECT * FROM " + table + ";";
            rc = sqlite3_exec(db, sqlCode.c_str(), selectCallback, &data, NULL);
    
            if (rc != SQLITE_OK) {
                handleSQLiteError(db, "Failed to execute SQL code");
                sqlite3_close(db);
                return "";
            }
    
            sqlite3_close(db);
            return "";
        }
    };
}

//// Hauptprogramm
//int main(int argc, char* argv[]) {
//    std::string data;
//    char* errorMessage = nullptr;
//
//    dblib::Database database("test.db");
//    std::string tableDefinition = "ID INTEGER PRIMARY KEY AUTOINCREMENT, "
//        "NAME TEXT NOT NULL, "
//        "LNAME TEXT NOT NULL, "
//        "AGE INT NOT NULL, "
//        "ADDRESS CHAR(50), "
//        "GRADE CHAR(1)";
//
//    std::string test = database.selectData("GRADES", "NAME", "AGE = 35");
//    std::cout << test << std::endl;
//    // database.createTable("GRADES", tableDefinition);
//    // database.selectAllData("GRADES");
//    // database.insertData("GRADES", "NAME, LNAME, AGE, ADDRESS, GRADE", "'Alice', 'Chapa', 35, 'Tampa', 'A'");
//    return 0;
//}

*/