#ifndef PARSER_H
#define PARSER_H
#include <string>
#include <vector>
#include <variant>
#include "tokenizer.h"

using Value = std::variant<int, std::string>;

// struct Column {
//     std::string name;
//     enum Type {INTEGER, TEXT } type;
// };

// struct Table {
//     std::string name;
//     const Column[] columns;
//     std::vector<Value[]> rows; 
// };

struct SelectCommand {
    std::vector<std::string> columns;
    bool all = false;
    std::string table_name;
    bool has_where_condition = false;
    std::string where_column;
    std::string where_value;

    SelectCommand() : columns(0) {}
};

struct CreateTableCommand {
    std::string name;
};

struct InsertCommand {
    std::string destination;
    std::vector<Value> values;
};

struct DropTableCommand {
    std::string table;
};

struct DeleteFromCommand {
    std::string table;
    std::string where_condition;
};

using Command = std::variant<SelectCommand, CreateTableCommand, InsertCommand, DropTableCommand, DeleteFromCommand>;

class Parser {
public:
    std::vector<Token> tokenized_query;
    Parser(std::vector<Token>& tokenized_query);
    Command get_commands();
    static bool skipSymbols(std::vector<Token>::iterator& it, const std::vector<Token>::iterator& end);
};


#endif