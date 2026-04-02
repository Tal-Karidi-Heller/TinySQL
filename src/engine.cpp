//
// Created by Tal Karidi on 21/03/2026.
//

#include "engine.h"

#include <cxxabi.h>
#include <iostream>
#include <string>
#ifdef _WIN32
#include <windows.h>
#else
#include <sys/ioctl.h>
#include <unistd.h>
#endif


int getTerminalWidth() {
    return 80;
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


bool Engine::table_exists(const std::string &name) {
    for (const auto &[key, value]: this->tables_map) {
        if (key == name) {
            return true;
        }
    }
    return false;
}

Engine::Engine() {
    this->tables_map = std::unordered_map<std::string, Table>(0);
}

void Engine::preetty_print_table(const std::vector<std::string> &columns, std::vector<std::vector<Value> > &rows) {
    int max_column_width = getTerminalWidth() / columns.size();

    for (int i = 0; i < columns.size(); i++)
        for (int j = 0; j < max_column_width; j++)
            std::cout << '-';

    std::cout << std::endl;

    for (const std::string &c: columns) {
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

    for (int i = 0; i < columns.size(); i++)
        for (int j = 0; j < max_column_width; j++)
            std::cout << '-';

    std::cout << std::endl;
}

bool Engine::evaluate_logical_condition(Table table, LogicalCondition logical_condition, std::vector<Value> row) {
    if (logical_condition.conditions.size() == 0) {
        throw std::invalid_argument("NO conditions");
    }

    std::optional<bool> result = std::nullopt;

    for (auto &c: logical_condition.conditions) {
        if (std::holds_alternative<SimpleCondition>(c)) {
            auto &simple_c = std::get<SimpleCondition>(c);
            auto r = table.get_column(simple_c.column);
            if (!r.has_value())
                throw std::invalid_argument(
                    "Column " + simple_c.column + "does not exist in table \"" + table.name + "\"");

            const auto [column_obj, column_index] = r.value();

            if (std::holds_alternative<int>(simple_c.value) != (column_obj.type == Column::INTEGER))
                throw std::invalid_argument("Type mismatch between usage of " + simple_c.column);

            bool _r = row[column_index] == simple_c.value;
            if (!result.has_value())
                result = _r;
            else {
                if (logical_condition.op == LogicalOperator::OR)
                    result = result.value() || _r;
                else
                    result = result.value() && _r;
            }
        } else {
            // This means that it holds another LogicalCondition
            auto &logical_c = std::get<LogicalCondition>(c);
            bool _r = evaluate_logical_condition(table, logical_c, row);

            if (!result.has_value())
                result = _r;
            else {
                if (logical_condition.op == LogicalOperator::OR)
                    result = result.value() || _r;
                else
                    result = result.value() && _r;
            }
        }
    }

    return result.value();
}

Status Engine::execute_command(const Command &command) {
    if (std::holds_alternative<SelectCommand>(command)) {
        SelectCommand select_command = std::get<SelectCommand>(command);
        if (!table_exists(select_command.table_name)) {
            throw std::invalid_argument("Table does not exist");
        }

        Table &table = tables_map.at(select_command.table_name);
        for (std::string column: select_command.columns)
            if (!table.get_column(column).has_value())
                throw std::invalid_argument("Column does not exist in table");


        std::vector<std::vector<Value> > output_rows;
        // TODO: We need to add a verification of the where clause.
        for (std::vector<Value> &row: table.rows) {
            bool where = (select_command.where.has_value())
                             ? evaluate_logical_condition(table, select_command.where.value(), row)
                             : true;

            if (where) {
                if (select_command.all)
                    output_rows.push_back(row);
                else {
                    std::vector<Value> _row;
                    _row.reserve(select_command.columns.size());
                    for (std::string &c: select_command.columns) {
                        const auto [column_obj, column_index] = table.get_column(c).value();
                        _row.push_back(row[column_index]);
                    }
                    output_rows.push_back(_row);
                }
            }
        }

        std::vector<std::string> columns = std::vector<std::string>();
        columns.reserve(table.columns.size());
        for (Column &c: table.columns)
            columns.push_back(c.name);


        this->preetty_print_table(
            (select_command.all) ? table.get_columns_string() : select_command.columns,
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

        Table table = Table(
            create_table_command.name,
            create_table_command.columns
        );

        this->tables_map.insert(
            {
                create_table_command.name,
                table
            }
        );

        auto empty = std::vector<std::vector<Value> >(0);

        for (const auto &[key, value]: this->tables_map) {
            std::cout << key << ", ";
        }

        return Status(
            true,
            "Created table",
            empty
        );
    } else if (std::holds_alternative<InsertCommand>(command)) {
        InsertCommand insert_command = std::get<InsertCommand>(command);
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
    } else if (std::holds_alternative<DeleteFromCommand>(command)) {
        DeleteFromCommand delete_from_command = std::get<DeleteFromCommand>(command);
        if (!table_exists(delete_from_command.table))
            throw std::invalid_argument("Table does not exist in database.");

        Table &table = tables_map.at(delete_from_command.table);
        std::vector<std::vector<Value> > rows_deleted;
        for (auto it = table.rows.begin(); it != table.rows.end();) {
            if (evaluate_logical_condition(table, delete_from_command.where, *it)) {
                // Meaning we need to erase that row.
                rows_deleted.push_back(*it);
                it = table.rows.erase(it);
            } else {
                ++it;
            }
        }

        this->preetty_print_table(
            table.get_columns_string(), rows_deleted
        );

        return Status(
            true,
            "DELETED The Following lines",
            rows_deleted
        );
    } else {
        return Status(
            false,
            "Command does not exist"
        );
    }
}

void Engine::save_table_to_file(std::ostream &file, const Table &table) {
    file << "TABLE DEFINITION " << table.name << std::endl;

    file << "COLUMNS [ ";
    for (int i = 0; i < table.columns.size(); i++) {
        file << table.columns[i].name << " " << table.columns[i].type;
        if (i < table.columns.size() - 1)
            file << ',';
    }
    file << " ]";

    file << std::endl;

    for (auto const &row: table.rows) {
        file << row << std::endl;
    }

    file << "END" << std::endl;
}

void Engine::save_to_file(std::ostream &file) {
    // Define columns
    for (const auto &[table_name, table]: this->tables_map) {
        save_table_to_file(file, table);
    }
}

void Engine::load_from_file(std::ifstream &file) {
    if (file.is_open()) {
        std::cout << "STARTED" << std::endl;
        std::string line;
        std::optional<Table> current_table = std::nullopt;
        while (std::getline(file, line)) {
            std::cout << line << std::endl;
            std::vector<Token> v = tokenize_query(line);
            VectorIterator<Token> it = VectorIterator<Token>(v.begin(), v.end());

            std::cout << it.peek() << std::endl;

            if (it.empty())
                continue;

            if (it.peek().value == "TABLE") {
                it.next(); // TAKING OUT TABLE
                it.next(); // TAKING OUT DEFINITION

                std::string table_name = it.next().value;
                current_table = Table(
                    table_name
                );
            } else if (it.peek().value == "COLUMNS") {
                // Columns definitions
                it.next();
                if (it.empty() || it.peek().value != "[")
                    throw std::runtime_error("INVALID FILE");

                it.next();

                std::vector<Column> columns;
                while (it.not_empty()) {
                    std::string column_name = it.next().value;
                    std::string column_type = it.next().value;

                    if (column_type != "TEXT" && column_type != "INTEGER")
                        throw std::runtime_error("INVALID COLUMN TYPE");

                    columns.push_back(
                        Column(
                            column_name,
                            (column_type == "TEXT") ? Column::TEXT : Column::INTEGER
                        )
                    );

                    it.next(); // Expecting , or ]
                }

                if (!current_table.has_value())
                    throw std::runtime_error("COLUMNS DEFINITION before TABLE DEFINITION");
                current_table.value().columns = columns;
            } else if (it.peek().value == "[") {
                it.next();
                // THIS IS A ROW.
                std::vector<Value> values;
                while (it.not_empty() && it.peek().value != "]") {
                    values.push_back(
                        (it.peek().type == Token::STRING_LITERAL)
                            ? Value(it.peek().value)
                            : Value(convert_to_number(it.peek().value).value())
                    );

                    it.next();

                    if (it.peek().value == ",")
                        it.next();
                }

                if (!current_table.has_value())
                    throw std::runtime_error("ROW APPEND before TABLE DEFINITION");

                current_table.value().rows.push_back(values);
            }
            else if (it.peek().value == "END") {
                it.next();
                if (!current_table.has_value())
                    throw std::runtime_error("END Before TABLE DEFINITION");

                if (this->table_exists(current_table.value().name))
                    throw std::runtime_error("TABLE ALREADY EXISTS");

                this->tables_map.insert(
                    {
                        current_table.value().name,
                        current_table.value()
                    }
                );
                std::cout << "ADDED TABLE " << current_table.value().name << std::endl;
            }
        }

        file.close();
    } else {
        std::cout << "HAVEN'T STARTED" << std::endl;
    }
}

std::vector<std::string> Engine::list_tables() {
    std::vector<std::string> output;
    for (const auto &[key, value]: this->tables_map)
        output.push_back(key);

    return output;
}
