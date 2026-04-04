#ifndef MY_PROJECT_COMMANDS_H
#define MY_PROJECT_COMMANDS_H
#include "conditions.h"
#include "utils.h"

struct CreateTableCommand {
    std::string name;
    std::vector<Column> columns;

    CreateTableCommand(const std::string &name, const std::vector<Column> &columns): name(name), columns(columns) {}

    CreateTableCommand(): name(""), columns() {}
};

struct InsertCommand {
    std::string destination;
    std::vector<Value> values;

    InsertCommand() {}
    InsertCommand(const std::string &destination, const std::vector<Value> &values): destination(destination), values(values) {}
};

struct DropTableCommand {
    std::string table;

    DropTableCommand(const std::string &table): table(table) {}
    DropTableCommand(): table("") {}
};

struct SelectCommand {
    std::vector<std::string> columns;
    bool all = false;
    std::string table_name;
    std::optional<LogicalCondition> where;

    SelectCommand() {}
    SelectCommand(const std::vector<std::string> &columns, const bool all, const std::string &table_name) : columns(columns), all(all), table_name(table_name) {}
};

inline std::ostream& operator<<(std::ostream &os, const SelectCommand c) {
    os << "[SelectCommand] {columns = " << c.columns << ", all = " << c.all << ", table_name = " << c.table_name << ", where = " << c.where << "}";
    return os;
}

struct DeleteFromCommand {
    std::string table;
    LogicalCondition where;

    DeleteFromCommand(std::string &table, LogicalCondition &where): table(table), where(where) {}
};

using Command = std::variant<SelectCommand, CreateTableCommand, InsertCommand, DropTableCommand, DeleteFromCommand>;


#endif
