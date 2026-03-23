#ifndef MY_PROJECT_TABLE_H
#define MY_PROJECT_TABLE_H
#include <vector>
#include <string>
#include <variant>
#include <unordered_map>

using Value = std::variant<int, std::string>;

struct Column {
    std::string name;
    enum Type {INTEGER, TEXT } type;
};

struct Table {
    std::string name;
    std::vector<Column> columns;
    std::vector<std::vector<Value>> rows;

    std::optional<std::tuple<Column, int>> get_column(std::string& column_name) {
        int i = 0;
        for (Column& c : this->columns) {
            if (c.name == column_name) {
                return std::tuple<Column, int>(c, i);
            }
            i++;
        }
        return std::nullopt;
    }

    Table(const std::string& name, const std::vector<Column>& columns) {
        this->name = name;
        this->columns = columns;
        this->rows = std::vector<std::vector<Value>>(0);
    }
};

#endif