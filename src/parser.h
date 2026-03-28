#ifndef PARSER_H
#define PARSER_H
#include <string>
#include <vector>
#include <variant>
#include "tokenizer.h"
#include "table.h"
#include "utils.h"

using Value = std::variant<int, std::string>;

std::ostream &operator<<(std::ostream &os, const Value &value);


struct SelectCommand {
    std::vector<std::string> columns;
    bool all = false;
    std::string table_name;
    // std::optional<Expression> where_expression;
};


struct CreateTableCommand {
    std::string name;
    std::vector<Column> columns;
};

struct InsertCommand {
    std::string destination;
    std::vector<Value> values;

    InsertCommand() {
    }
};

struct DropTableCommand {
    std::string table;
};

struct DeleteFromCommand {
    std::string table;
    // std::optional<Expression> where_expression;
};

using Command = std::variant<SelectCommand, CreateTableCommand, InsertCommand, DropTableCommand, DeleteFromCommand>;

enum LogicalOperator { AND, OR };

struct SimpleCondition {
    std::string column;

    enum EqualOp { EQUALS };

    EqualOp equal_op;
    Value value;

    SimpleCondition(const std::string &column, EqualOp equal_op, const Value &value) : column(column),
        equal_op(equal_op), value(value) {
    }
};

inline std::ostream &operator<<(std::ostream &os, const LogicalOperator &s) {
    if (s == LogicalOperator::AND)
        os << "AND";
    else
        os << "OR";
    return os;
}

inline std::ostream &operator<<(std::ostream &os, const SimpleCondition &s) {
    os << s.column << " = " << s.value;
    return os;
}

struct LogicalCondition {
    LogicalOperator op;
    std::vector<std::variant<SimpleCondition, LogicalCondition> > conditions;

    LogicalCondition(LogicalOperator op) : op(op) {
    }

    LogicalCondition(LogicalOperator op, SimpleCondition condition) : op(op) {
        conditions.push_back(condition);
    }
};

inline std::ostream &operator<<(std::ostream &os, const LogicalCondition &l) {
    int i = 0;
    os << '(';
    for (auto _r : l.conditions) {
        if (std::holds_alternative<SimpleCondition>(_r)) {
            auto r2 = std::get<SimpleCondition>(_r);
            os << r2;

        }
        else {
            auto r2 = std::get<LogicalCondition>(_r);
            os << r2;
        }

        if (i < l.conditions.size() - 1)
            os << ' ' << l.op << ' ';

        i++;
    }
    os << ')';

    return os;
}

enum Parentheses { OPEN };


class Parser {
private:
    static Column get_column(std::vector<Token>::iterator &it, std::vector<Token>::iterator end);

public:
    std::vector<Token> tokenized_query;

    Parser(std::vector<Token> &tokenized_query);

    Command get_commands();

    static LogicalCondition parse_where(VectorIterator<Token> &token_iterator);
};

#endif
