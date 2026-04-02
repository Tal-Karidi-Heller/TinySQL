#ifndef MY_PROJECT_TABLE_H
#define MY_PROJECT_TABLE_H
#include <vector>
#include <string>
#include <variant>
#include <iostream>
#include <unordered_map>

using Value = std::variant<int, std::string>;

inline std::ostream& operator<<(std::ostream &os, const Value &v) {
    if (std::holds_alternative<int>(v))
        os << std::get<int>(v);
    else
        os << '"' << std::get<std::string>(v) << '"';

    return os;
}

struct Column {
    std::string name;
    enum Type {INTEGER, TEXT } type;

    Column(const std::string &name, const enum Type type) : name(name), type(type) {}
};

inline std::ostream& operator<<(std::ostream &os, const Column::Type t) {
    if (t == Column::INTEGER)
        os << "INTEGER";
    else if (t == Column::TEXT)
        os << "TEXT";
    return os;
}

inline std::ostream& operator<<(std::ostream &os, const Column c) {
    os << "{ name = " << c.name << " , type = " << c.type << " }";
    return os;
}

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

    inline std::vector<std::string> get_columns_string() {
        std::vector<std::string> output;
        for (const Column &c : columns)
            output.push_back(c.name);
        return output;
    }

    Table(const std::string& name, const std::vector<Column>& columns): name(name), columns(columns) {}

    Table(const std::string& name): name(name) {}
};

#endif