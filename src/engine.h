//
// Created by Tal Karidi on 21/03/2026.
//

#ifndef MY_PROJECT_ENGINE_H
#define MY_PROJECT_ENGINE_H
#include <vector>
#include <string>
#include <variant>
#include <unordered_map>
#include "parser.h"
#include "table.h"



struct Status {
    bool succeeded;
    std::string description;
    std::optional<std::vector<std::vector<Value>>> output_rows;

    Status(const bool& succeeded, const std::string& description, std::vector<std::vector<Value>>& output_rows) {
        this->succeeded = succeeded;
        this->description = description;
        this->output_rows = output_rows;
    }
};

std::ostream &operator<<(std::ostream &os, Status& status);

std::ostream& operator<<(std::ostream& os, std::vector<std::vector<Value>> &rows);


class Engine {
private:
    std::unordered_map<std::string, Table> tables_map;
public:
    Engine();
    bool table_exists(const std::string& name);

    static void preetty_print_table(std::vector<std::string> &columns, std::vector<std::vector<Value>> &rows);
    Status save_to_disk();
    Status load_from_disk();
    Status execute_command(const Command& command);
};


#endif