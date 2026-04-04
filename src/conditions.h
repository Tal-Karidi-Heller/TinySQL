//
// Created by Tal Karidi on 03/04/2026.
//

#ifndef MY_PROJECT_CONDITIONS_H
#define MY_PROJECT_CONDITIONS_H
#include <string>
#include <iostream>
#include "table.h"

enum LogicalOperator { AND, OR };

enum Parentheses { OPEN };

struct SimpleCondition {
    std::string column;

    enum EqualOp { EQUALS };

    EqualOp equal_op;
    Value value;

    SimpleCondition(const std::string &column, EqualOp equal_op, const Value &value) : column(column),
        equal_op(equal_op), value(value) {
    }

    SimpleCondition() : column(""), equal_op(EqualOp::EQUALS), value("") {
    }

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
    std::vector<std::variant<SimpleCondition, LogicalCondition> > conditions;

    LogicalCondition(LogicalOperator op) : op(op), conditions(0) {
    }

    LogicalCondition(const LogicalOperator &op,
                     const std::vector<std::variant<SimpleCondition, LogicalCondition> > &
                     conditions) : op(op), conditions(conditions) {
    }

    LogicalCondition(LogicalOperator op, SimpleCondition condition) : op(op), conditions() {
        conditions.push_back(condition);
    }
};

inline std::ostream &operator<<(std::ostream &os, const LogicalCondition &l) {
    int i = 0;
    os << '(';
    for (auto _r: l.conditions) {
        if (std::holds_alternative<SimpleCondition>(_r)) {
            auto r2 = std::get<SimpleCondition>(_r);
            os << r2;
        } else {
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

#endif //MY_PROJECT_CONDITIONS_H
