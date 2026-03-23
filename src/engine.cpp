//
// Created by Tal Karidi on 21/03/2026.
//

#include "engine.h"
#include <iostream>

// Engine::Engine() = default;

std::ostream &operator<<(std::ostream &os, Status &status) {
    os << "succeeded = " << status.succeeded << ", description = " << status.description;
    return os;
}

std::ostream& operator<<(std::ostream& os, Value value) {
    std::string value_string = (std::holds_alternative<int>(value)) ? std::to_string(std::get<int>(value)) : std::get<std::string>(value);
    os << value_string;
    return os;
}

std::ostream& operator<<(std::ostream& os, std::vector<std::vector<Value>> &rows) {
    for (const auto &row : rows) {
        for (const auto &value : row) {
            os << value << ", ";
        }
        os << std::endl;
    }
    return os;
}

bool Engine::table_exists(const std::string &name) {
    std::cout << "starting" << std::endl;
    for (const auto & [key, value]: this->tables_map) {
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


Status Engine::execute_command(const Command &command) {
    if (std::holds_alternative<SelectCommand>(command)) {
        SelectCommand select_command = std::get<SelectCommand>(command);
        if (!table_exists(select_command.table_name)) {
            throw std::invalid_argument("Table does not exist");
        }

        Table &table = tables_map.at(select_command.table_name);

        // Validate where_conditions:
        for (SingleCondition &condition: select_command.where_conditions) {
            std::optional<std::tuple<Column, int> > table_column = table.get_column(condition.where_column);

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

            if (passed)
                output_rows.push_back(row);
        }

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

        auto empty = std::vector<std::vector<Value>>(0);

        std::cout << "tables_map_keys: " << std::endl;
        for (const auto & [key, value] : this->tables_map) {
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
        for (const auto & [key, value] : this->tables_map) {
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

            std::cout << "column type = " << table.columns[i].type << ", TEXT = " << Column::TEXT << ", is_int = " << std::holds_alternative<int>(insert_command.values[i]) << std::endl;

            if (query_column_type != table.columns[i].type) {
                std::string value_string = (std::holds_alternative<int>(insert_command.values[i]))
                                               ? std::to_string(std::get<int>(insert_command.values[i]))
                                               : std::get<std::string>(insert_command.values[i]);
                throw std::invalid_argument("Wrong Value Type for column " + table.columns[i].name + ", with value = " + value_string + "=");
            }
        }

        // If we got here it means that types are validated. We can safely add the row.
        table.rows.push_back(insert_command.values);

        auto empty = std::vector<std::vector<Value>>(0);

        return Status(
            true,
            "Added row",
            empty
        );
    } else {
    }
}
