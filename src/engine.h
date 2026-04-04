//
// Created by Tal Karidi on 21/03/2026.
//

#ifndef MY_PROJECT_ENGINE_H
#define MY_PROJECT_ENGINE_H
#include <vector>
#include <string>
#include <variant>
#include <unordered_map>
#include <fstream>
#include "parser.h"
#include "table.h"


struct Status {
    bool succeeded;
    std::string description;
    std::optional<std::vector<std::vector<Value> > > output_rows;

    Status(const bool &succeeded, const std::string &description,
           std::vector<std::vector<Value> > &output_rows) : succeeded(succeeded), description(description),
                                                            output_rows(output_rows) {
    }

    Status(const bool &succeeded, const std::string &description) : succeeded(succeeded), description(description),
                                                                    output_rows(std::nullopt) {
    }
};

class EngineException : public ExpectedException {
    std::string message;

public:
    explicit  EngineException(const std::string &message) : message("[ENGINE] " + message) {}

    const char* what() const noexcept override {
        return message.c_str();
    }
};

class LoadException : public ExpectedException {
    std::string message;

public:
    explicit  LoadException(const std::string &message) : message("[ENGINE_LOAD]" + message) {}

    const char* what() const noexcept override {
        return message.c_str();
    }
};

class Engine {
private:
    std::unordered_map<std::string, Table> tables_map;

public:
    Engine();

    bool table_exists(const std::string &name);

    void load_from_file(std::ifstream &file);

    void save_to_file(std::ostream &file);

    static void save_table_to_file(std::ostream &file, const Table &table);


    static void pretty_print_table(const std::vector<std::string> &columns, std::vector<std::vector<Value> > &rows);

    static bool evaluate_logical_condition(Table &table, LogicalCondition &logical_condition, std::vector<Value> &row);

    Status execute_command(const Command &command);


    std::vector<std::string> list_tables();
};


#endif
