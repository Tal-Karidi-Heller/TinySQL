//
// Created by Tal Karidi on 21/03/2026.
//

#include "engine.h"
#include <iostream>
#include <string>
#ifdef _WIN32
#include <windows.h>
#else
#include <sys/ioctl.h>
#include <unistd.h>
#endif


int getTerminalWidth() {
#ifdef _WIN32
    CONSOLE_SCREEN_BUFFER_INFO csbi;
    if (GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &csbi)) {
        return csbi.srWindow.Right - csbi.srWindow.Left + 1;
    }
#else
    struct winsize w;
    if (ioctl(STDOUT_FILENO, TIOCGWINSZ, &w) == 0) {
        return w.ws_col;
    }
#endif
    return 80; // Default fallback
}



std::ostream &operator<<(std::ostream &os, Status &status) {
    os << "succeeded = " << status.succeeded << ", description = " << status.description;
    return os;
}

std::ostream &operator<<(std::ostream &os, Value value) {
    std::string value_string = (std::holds_alternative<int>(value))
                                   ? std::to_string(std::get<int>(value))
                                   : std::get<std::string>(value);
    os << value_string;
    return os;
}

std::ostream &operator<<(std::ostream &os, std::vector<std::vector<Value> > &rows) {
    for (const auto &row: rows) {
        for (const auto &value: row) {
            os << value << ", ";
        }
        os << std::endl;
    }
    return os;
}

bool Engine::table_exists(const std::string &name) {
    std::cout << "starting" << std::endl;
    for (const auto &[key, value]: this->tables_map) {
        std::cout << "key = " << key;
        if (key == name) {
            return true;
        }
    }
    return false;
}

Engine::Engine() {
    this->tables_map = std::unordered_map<std::string, Table>(0);
}

void Engine::preetty_print_table(std::vector<std::string> &columns, std::vector<std::vector<Value> > &rows) {
    int max_column_width = getTerminalWidth() / columns.size();

    std::cout << columns.size() << std::endl;

    for (int i = 0; i < columns.size(); i++)
        for (int j = 0; j < max_column_width; j++)
            std::cout << '-';

    for (std::string &c: columns) {
        if (c.size() > max_column_width) {
            std::cout << c.substr(max_column_width - 3) << "...";
        } else {
            std::cout << c;
            for (int s = 0; s < max_column_width - c.size() - 1; s++)
                std::cout << ' ';
        }
        std::cout << '|';
    }
    std::cout << std::endl;

    for (int i = 0; i < columns.size(); i++)
        for (int j = 0; j < max_column_width; j++)
            std::cout << '-';

    std::cout << std::endl;

    for (int row_i = 0; row_i < rows.size(); row_i++) {
        std::vector<Value> &c_row = rows[row_i];
        if (columns.size() != c_row.size()) {
            throw std::invalid_argument("Columns length not matching rows columns");
        }
        for (int col_i = 0; col_i < c_row.size(); col_i++) {
            std::string _value = (std::holds_alternative<std::string>(c_row[col_i]))
                                     ? std::get<std::string>(c_row[col_i])
                                     : std::to_string(std::get<int>(c_row[col_i]));

            if (_value.size() > max_column_width) {
                std::cout << _value.substr(0, max_column_width - 3 - 1) << "...";
            } else {
                std::cout << _value;
                for (int s = 0; s < max_column_width - _value.size() - 1; s++)
                    std::cout << ' ';
            }

            std::cout << '|';
        }
        std::cout << std::endl;
    }
}


Status Engine::execute_command(const Command &command) {
    if (std::holds_alternative<SelectCommand>(command)) {
        SelectCommand select_command = std::get<SelectCommand>(command);
        if (!table_exists(select_command.table_name)) {
            throw std::invalid_argument("Table does not exist");
        }

        Table &table = tables_map.at(select_command.table_name);

        // TODO: Validate SELECT columns.

        // Validate where_conditions:
        for (SingleCondition &condition: select_command.where_conditions) {
            std::optional<std::tuple<Column, int> > table_column = table.get_column(condition.where_column);
            // TODO: Add support for condition.operator and also (a == 2 and b == 3 or (a == 5 and b == 2)).
            // How should I represent this?

            if (table_column.has_value() == false)
                throw std::invalid_argument(
                    "Select Command contains where condition with a column that does not exist in table: " + condition.
                    where_column);

            Column column = std::get<0>(table_column.value());



            if ((column.type == Column::TEXT && std::holds_alternative<int>(condition.where_value)) || (
                    column.type == Column::INTEGER && std::holds_alternative<std::string>(
                        condition.where_value)))
                throw std::invalid_argument("Where condition " + column.name + " has wrong type condition");
        }

        std::vector<std::vector<Value> > output_rows;

        for (std::vector<Value> &row: table.rows) {
            bool passed = true;
            for (SingleCondition &condition: select_command.where_conditions) {
                const auto [column, index] = table.get_column(condition.where_column).value();
                // Because we already checked that this column exists in th Validate where_conditions

                if (row[index] != condition.where_value) {
                    passed = false;
                }
            }

            if (passed) {
                if (select_command.all)
                    output_rows.push_back(row);
                else {
                    std::vector<Value> _row;
                    _row.reserve(select_command.columns.size());
                    for (std::string &c: select_command.columns) {
                        const auto [column_obj, column_index] = table.get_column(c).value();
                        _row.push_back(row[column_index]);
                    }
                }
            }
        }

        std::vector<std::string> columns = std::vector<std::string>();
        columns.reserve(table.columns.size());
        for (Column &c: table.columns) {
            std::cout << c.name << ", ";
            columns.push_back(c.name);
        }
        std::cout << std::endl;


        this->preetty_print_table(
            columns,
            output_rows
        );

        return Status(
            true,
            "Select command executed successfully",
            output_rows
        );
    } else if (std::holds_alternative<CreateTableCommand>(command)) {
        CreateTableCommand create_table_command = std::get<CreateTableCommand>(command);

        if (table_exists(create_table_command.name))
            throw std::invalid_argument("Table " + create_table_command.name + " already exists");

        std::cout << "Trying to initialize table" << std::endl;
        Table table = Table(
            create_table_command.name,
            create_table_command.columns
        );

        std::cout << "Initalized table" << std::endl;

        std::cout << this->tables_map.size() << std::endl;

        this->tables_map.insert(
            {
                create_table_command.name,
                table
            }
        );

        auto empty = std::vector<std::vector<Value> >(0);

        std::cout << "tables_map_keys: " << std::endl;
        for (const auto &[key, value]: this->tables_map) {
            std::cout << key << ", ";
        }
        std::cout << std::endl;

        return Status(
            true,
            "Created table",
            empty
        );
    } else if (std::holds_alternative<InsertCommand>(command)) {
        InsertCommand insert_command = std::get<InsertCommand>(command);
        std::cout << "tables_map_keys: " << std::endl;
        for (const auto &[key, value]: this->tables_map) {
            std::cout << key << ", ";
        }
        std::cout << std::endl;
        if (table_exists(insert_command.destination) == false)
            throw std::invalid_argument("Table does not exist.");

        // Validate types:
        Table &table = this->tables_map.at(insert_command.destination);
        if (table.columns.size() != insert_command.values.size())
            throw std::invalid_argument("InsertCommand must contain values for each column. Not less Not more.");

        for (int i = 0; i < table.columns.size(); i++) {
            Column::Type query_column_type = (std::holds_alternative<int>(insert_command.values[i]))
                                                 ? Column::INTEGER
                                                 : Column::TEXT;

            std::cout << "column type = " << table.columns[i].type << ", TEXT = " << Column::TEXT << ", is_int = " <<
                    std::holds_alternative<int>(insert_command.values[i]) << std::endl;

            if (query_column_type != table.columns[i].type) {
                std::string value_string = (std::holds_alternative<int>(insert_command.values[i]))
                                               ? std::to_string(std::get<int>(insert_command.values[i]))
                                               : std::get<std::string>(insert_command.values[i]);
                throw std::invalid_argument(
                    "Wrong Value Type for column " + table.columns[i].name + ", with value = " + value_string + "=");
            }
        }

        // If we got here it means that types are validated. We can safely add the row.
        table.rows.push_back(insert_command.values);

        auto empty = std::vector<std::vector<Value> >(0);

        return Status(
            true,
            "Added row",
            empty
        );
    } else {
    }
}
