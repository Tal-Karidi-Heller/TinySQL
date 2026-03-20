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

struct SingleCondition {
    std::string where_column;
    std::string where_value;
    
    enum Operator {AND, OR, NONE};
    Operator op;

    SingleCondition() : op(Operator::NONE) {

    }
};

struct SelectCommand {
    std::vector<std::string> columns;
    bool all = false;
    std::string table_name;
    bool has_where_condition = false;
    std::vector<SingleCondition> where_conditions;

    SelectCommand() : columns(0), where_conditions(0) {}
};


struct CreateTableCommand {
    std::string name;
};

struct InsertCommand {
    std::string destination;
    std::vector<std::string> values;

    InsertCommand() : values(0) {}
};

struct DropTableCommand {
    std::string table;
};

struct DeleteFromCommand {
    std::string table;
    std::vector<SingleCondition> where_condition;

    DeleteFromCommand() : where_condition(0) {}
};

using Command = std::variant<SelectCommand, CreateTableCommand, InsertCommand, DropTableCommand, DeleteFromCommand>;

class Parser {
public:
    std::vector<Token> tokenized_query;
    Parser(std::vector<Token>& tokenized_query);
    Command get_commands();
    static bool skipSymbols(std::vector<Token>::iterator& it, const std::vector<Token>::iterator& end);
    
    static std::vector<SingleCondition> _get_where_conditions(std::vector<Token>::iterator& start, std::vector<Token>::iterator& end, std::vector<SingleCondition>& output);
    
    static std::vector<SingleCondition> get_where_conditions(std::vector<Token>::iterator start, std::vector<Token>::iterator end);
};

std::ostream &operator<<(std::ostream &os, const std::vector<std::string> &vector);
std::ostream &operator<<(std::ostream &os, const SingleCondition &condition);
std::ostream &operator<<(std::ostream &os, const std::vector<SingleCondition> &conditions);
std::ostream &operator<<(std::ostream &os, const SelectCommand &command);

#endif