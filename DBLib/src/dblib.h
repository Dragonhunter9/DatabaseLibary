#include <iostream>
#include <string>
#include "sqlite/sqlite3.h"

constexpr std::string DBLIB_TEXT = "TEXT";
constexpr std::string DBLIB_INT = "INT";
constexpr std::string DBLIB_CHAR = "CHAR";

namespace dblib {
    enum DBLIBTYPE {
        TEXT = 0, INT, CHAR
    };

    class Database {
    protected:
        sqlite3* db;
        std::string path;

        // Callback-Funktion für die Ausgabe von ausgewählten Daten
        static int selectCallback(void* data, int argc, char** argv, char** azColName);

        void handleSQLiteError(sqlite3* db, const std::string& message) const;

        bool executeSQL(const std::string& sql);

    public:
        // Create a new database
        Database(const std::string& filePath);

        Database(const Database&) = delete;
        void operator=(const Database&) = delete;

        // SQL: CREATE TABLE IF NOT EXISTS {table} ({tableDefinition});
        void createTable(const std::string& table, const std::string& tableDefinition);

        // SQL: INSERT INTO {table} ({columns}) VALUES ({values});
        void insertData(const std::string& table, const std::string& columns, const std::string& values);

        // SQL: UPDATE {table} SET {setValues} WHERE {condition};
        void updateData(const std::string& table, const std::string& setValues, const std::string& condition);

        // SQL: DELETE FROM {table} WHERE {condition};
        void deleteData(const std::string& table, const std::string& condition);

        // SQL: SELECT {columns} FROM {table} WHERE {condition};
        std::string selectData(const std::string& table, const std::string& columns, const std::string& condition);

        // SQL: SELECT * FROM {table};
        std::string selectAllData(const std::string& table);
    };

    //std::string tableDefinition = "ID INTEGER PRIMARY KEY AUTOINCREMENT, "
    //              "NAME TEXT NOT NULL, "
    //              "LNAME TEXT NOT NULL, "
    //              "ADDRESS CHAR(50), "
    //              "AGE INT NOT NULL, "
    //              "GRADE CHAR(1)";

    class TableDefinition {
    protected:
        std::string sqlCode;
    public:
        TableDefinition(bool hasID);

        template <DBLIBTYPE>
        void AddColumn(const std::string& name, bool isNullable);

        template <>
        void AddColumn<TEXT>(const std::string& name, bool isNullable);

        template <>
        void AddColumn<INT>(const std::string& name, bool isNullable);

        template<>
        void AddColumn<CHAR>(const std::string& name, bool isNullable);
    };
}