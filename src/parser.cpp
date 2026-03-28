#include <iostream>
#include "parser.h"
#include "tokenizer.h"
#include "utils.h"

template class VectorIterator<Token>;

std::ostream &operator<<(std::ostream &os, const Value &value) {
    if (std::holds_alternative<std::string>(value)) {
        std::cout << (std::get<std::string>(value));
    } else {
        std::cout << std::get<int>(value);
    }
    return os;
}

std::ostream &operator<<(std::ostream &os, const std::vector<std::string> &vector) {
    for (int i = 0; i < vector.size(); i++) {
        std::cout << vector[i];
        if (i < vector.size() - 1) {
            std::cout << ", ";
        }
    }

    return os;
}

std::ostream &operator<<(std::ostream &os, const std::vector<Column> &vector) {
    for (int i = 0; i < vector.size(); i++) {
        std::cout << vector[i].name << ", " << vector[i].type;
        if (i < vector.size() - 1) {
            std::cout << ", ";
        }
    }

    return os;
}

std::optional<int> convert_to_number(const std::string &string) {
    try {
        int num = std::stoi(string);
        return num;
    } catch (const std::invalid_argument &e) {
        return std::nullopt;
    } catch (const std::out_of_range &e) {
        return std::nullopt;
    }
}


Parser::Parser(std::vector<Token> &tokenized_query) {
    this->tokenized_query = tokenized_query;
}

SimpleCondition read_condition(VectorIterator<Token> &token_iterator) {
    if (!token_iterator.not_empty())
        throw std::invalid_argument("No column");

    std::string column = token_iterator.next().value;

    std::cout << "column = " << column << std::endl;

    if (!token_iterator.not_empty())
        throw std::invalid_argument("No =");

    std::string equal_op = token_iterator.next().value;

    if (!token_iterator.not_empty())
        throw std::invalid_argument("No value");

    Value value = token_iterator.next().value;

    return SimpleCondition(column, SimpleCondition::EQUALS, value);
}

LogicalCondition Parser::parse_where(VectorIterator<Token> &token_iterator) {
    std::stack<std::variant<Parentheses, LogicalCondition, LogicalOperator>> output;

    SimpleCondition c1 = read_condition(token_iterator);
    LogicalCondition current_condition = LogicalCondition(
        LogicalOperator::AND,
        c1
    );

    int np = 0;

    Token logical_op = token_iterator.next();

    while (token_iterator.not_empty()) {
        if (logical_op.value == "OR" && current_condition.conditions.size() > 0) {
            output.push(current_condition);
        }

        if (token_iterator.peek().value == "(") {
            np++;

            output.push(current_condition);
            if (logical_op.value == "AND")
                output.push(LogicalOperator::AND);

            output.push(Parentheses::OPEN);

            token_iterator.next();

            c1 = read_condition(token_iterator);
            current_condition = LogicalCondition(
                LogicalOperator::AND,
                c1
            );
        } else {
            c1 = read_condition(token_iterator);
            if (logical_op.value == "AND") {
                current_condition.conditions.push_back(c1);
            } else {
                current_condition = LogicalCondition(
                    LogicalOperator::AND,
                    c1
                );
            }
        }

        if (token_iterator.peek().value == ")") {
            std::cout << ") exists" << std::endl;
            if (np < 1)
                throw std::invalid_argument(") without (");
            np--;
            output.push(current_condition);
            std::cout << "( push: " << current_condition;

            LogicalCondition c = LogicalCondition(
                LogicalOperator::OR
            );
            std::cout << "output size: " << output.size() << std::endl;
            while (!std::holds_alternative<Parentheses>(output.top())) {
                c.conditions.push_back(std::get<LogicalCondition>(output.top()));
                output.pop();
            }
            output.pop();

            if (std::holds_alternative<LogicalOperator>(output.top())) {
                std::cout << "Have AND" << std::endl;
                LogicalCondition c_tag = LogicalCondition(
                    std::get<LogicalOperator>(output.top())
                );
                output.pop();

                c_tag.conditions.push_back(std::get<LogicalCondition>(output.top()));
                output.pop();
                c_tag.conditions.push_back(c);

                output.push(c_tag);
            }
            else
                output.push(c);

            token_iterator.next();
            current_condition = LogicalCondition(
                LogicalOperator::AND
            );
        }

        if (token_iterator.not_empty())
            logical_op = token_iterator.next();
    }

    if (current_condition.conditions.size() > 0)
        output.push(current_condition);

    LogicalCondition final_output = LogicalCondition(LogicalOperator::OR);
    std::cout << "np = " << np << std::endl;
    std::cout << "size = " << output.size() << std::endl;
    while (!output.empty()) {
        if (std::holds_alternative<Parentheses>(output.top()))
            throw std::invalid_argument("Still ) in output");

        final_output.conditions.push_back(std::get<LogicalCondition>(output.top()));
        output.pop();
    }

    std::cout << "Finished 123123" << std::endl;

    return final_output;
}


Column Parser::get_column(std::vector<Token>::iterator &it, std::vector<Token>::iterator end) {
    Column column;
    std::cout << it->value << std::endl;
    if (it->type != Token::Type::IDENTIFIER) {
        throw std::invalid_argument("");
    }

    column.name = it->value;
    ++it;

    if (it == end || (it->value != "INTEGER" && it->value != "TEXT")) {
        throw std::invalid_argument("Valid type (INTEGER or TEXT) must come after column name");
    }

    if (it->value == "INTEGER")
        column.type = Column::Type::INTEGER;
    else if (it->value == "TEXT")
        column.type = Column::Type::TEXT;

    return column;
}

Command Parser::get_commands() {
    if (tokenized_query.size() < 1) {
        throw std::invalid_argument("Query must contain tokens.");
    }

    VectorIterator<Token> it = VectorIterator<Token>(tokenized_query.begin(), tokenized_query.end());
    if (it.peek().value == "SELECT") {
    } else if (tokenized_query[0].value == "DROP") {
        std::vector<Token>::iterator it = tokenized_query.begin() + 1;
        std::vector<Token>::iterator end = tokenized_query.end();
        DropTableCommand command;
        if (it->value != "TABLE") {
            throw std::invalid_argument("TABLE keyword must come after DROP");
        }

        it++;

        if (it == end) {
            throw std::invalid_argument("Table name must come after TABLE keyword.");
        }

        command.table = it->value;

        std::cout << command.table << std::endl;

        return command;
    } else if (tokenized_query[0].value == "CREATE") {
        std::vector<Token>::iterator it = tokenized_query.begin() + 1;
        std::vector<Token>::iterator end = tokenized_query.end();
        CreateTableCommand create_table_command;

        if (it == end || it->value != "TABLE")
            throw std::invalid_argument("TABLE Must come after CREATE");

        ++it;

        if (it == end)
            throw std::invalid_argument("Table Name must come after TABLE");

        create_table_command.name = it->value;
        create_table_command.columns = std::vector<Column>(0);

        ++it;

        if (it == end || it->value != "(") {
            throw std::invalid_argument("Query must have(<col> <type>, ...)");
        }

        ++it;

        while (it != end && it->value != ")") {
            if (it->value == ",")
                ++it;
            Column c = get_column(it, end);
            create_table_command.columns.push_back(c);
            ++it;
        }

        std::cout << "columns: " << create_table_command.columns << std::endl;;

        return create_table_command;
    } else {
        throw std::invalid_argument("Not an available command.");
    }
}

