#ifndef PARSER_H
#define PARSER_H
#include <string>
#include <vector>
#include <variant>
#include <sys/time.h>

#include "tokenizer.h"
#include "table.h"
#include "utils.h"

struct CreateTableCommand {
    std::string name;
    std::vector<Column> columns;

    CreateTableCommand(const std::string &name, const std::vector<Column> &columns): name(name), columns(columns) {}

    CreateTableCommand(): name(""), columns() {}
};

struct InsertCommand {
    std::string destination;
    std::vector<Value> values;

    InsertCommand() {}
    InsertCommand(const std::string &destination, const std::vector<Value> &values): destination(destination), values(values) {}
};

struct DropTableCommand {
    std::string table;

    DropTableCommand(const std::string &table): table(table) {}
    DropTableCommand(): table("") {}
};


enum LogicalOperator { AND, OR };

struct SimpleCondition {
    std::string column;

    enum EqualOp { EQUALS };

    EqualOp equal_op;
    Value value;

    SimpleCondition(const std::string &column, EqualOp equal_op, const Value &value) : column(column),
        equal_op(equal_op), value(value) {
    }

    SimpleCondition() : column(""), equal_op(EqualOp::EQUALS), value("") {}

    inline bool operator==(SimpleCondition other) const {
        return this->equal_op == other.equal_op && this->value == other.value && this->column == other.column;
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
    std::vector<std::variant<SimpleCondition, LogicalCondition>> conditions;

    LogicalCondition(LogicalOperator op) : op(op), conditions(0) {}

    LogicalCondition(const LogicalOperator &op, const std::vector<std::variant<SimpleCondition, LogicalCondition>> &conditions): op(op), conditions(conditions) {}

    LogicalCondition(LogicalOperator op, SimpleCondition condition) : op(op), conditions() {
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

struct SelectCommand {
    std::vector<std::string> columns;
    bool all = false;
    std::string table_name;
    std::optional<LogicalCondition> where;

    SelectCommand() {}
    SelectCommand(const std::vector<std::string> &columns, const bool all, const std::string &table_name) : columns(columns), all(all), table_name(table_name) {}
};

inline std::ostream& operator<<(std::ostream &os, const SelectCommand c) {
    os << "[SelectCommand] {columns = " << c.columns << ", all = " << c.all << ", table_name = " << c.table_name << ", where = " << c.where << "}";
    return os;
}

struct DeleteFromCommand {
    std::string table;
    LogicalCondition where;

    DeleteFromCommand(std::string &table, LogicalCondition &where): table(table), where(where) {}
};

using Command = std::variant<SelectCommand, CreateTableCommand, InsertCommand, DropTableCommand, DeleteFromCommand>;


class Parser {
private:
    static Column get_column(VectorIterator<Token> &it);

public:
    std::vector<Token> tokenized_query;

    Parser(const std::vector<Token> &tokenized_query);

    explicit Parser(int _cpp_par_);

    Command get_command();

    static LogicalCondition parse_where(VectorIterator<Token> &token_iterator);
};

#endif
