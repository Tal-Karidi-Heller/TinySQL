#include <iostream>
#include "parser.h"
#include "tokenizer.h"
#include "utils.h"


Parser::Parser(const std::vector<Token> &tokenized_query) {
    this->tokenized_query = tokenized_query;
}

SimpleCondition read_condition(VectorIterator<Token> &token_iterator) {
    if (!token_iterator.not_empty())
        throw std::invalid_argument("No column");

    std::string column = token_iterator.next().value;

    if (!token_iterator.not_empty())
        throw std::invalid_argument("No =");

    std::string equal_op = token_iterator.next().value;

    if (!token_iterator.not_empty())
        throw std::invalid_argument("No value");

    Token c = token_iterator.next();
    Value value;;

    if (c.type == Token::NUMERIC_LITERAL)
        value = std::stoi(c.value);
    else
        value = c.value;

    return SimpleCondition(column, SimpleCondition::EQUALS, value);
}

LogicalCondition Parser::parse_where(VectorIterator<Token> &token_iterator) {
    if (!token_iterator.not_empty())
        throw std::invalid_argument("Where clause must come after WHERE");

    std::stack<std::variant<Parentheses, LogicalCondition, LogicalOperator> > output;

    SimpleCondition c1 = SimpleCondition();
    LogicalCondition current_condition = LogicalCondition(
        LogicalOperator::AND
    );

    int np = 0;

    Token logical_op = Token("", Token::UNDEFINED);

    while (token_iterator.not_empty()) {
        if (logical_op.value == "OR" && current_condition.conditions.size() > 0) {
            output.push(current_condition);
        }

        if (token_iterator.peek().value == "(") {
            np++;

            if (logical_op.value != "OR" && current_condition.conditions.size() > 0)
                // because we already pushed current_condition
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

        if (token_iterator.not_empty() && token_iterator.peek().value == ")") {
            if (np < 1)
                throw std::invalid_argument(") without (");
            np--;
            output.push(current_condition);

            LogicalCondition c = LogicalCondition(
                LogicalOperator::OR
            );
            while (!std::holds_alternative<Parentheses>(output.top())) {
                c.conditions.push_back(std::get<LogicalCondition>(output.top()));
                output.pop();
            }
            output.pop();

            if (!output.empty() && std::holds_alternative<LogicalOperator>(output.top())) {
                LogicalCondition c_tag = LogicalCondition(
                    std::get<LogicalOperator>(output.top())
                );
                output.pop();

                c_tag.conditions.push_back(std::get<LogicalCondition>(output.top()));
                output.pop();
                c_tag.conditions.push_back(c);

                // output.push(c_tag);
                current_condition = LogicalCondition(
                    LogicalOperator::AND
                );
                current_condition.conditions.push_back(c_tag);
            } else
                current_condition = LogicalCondition(
                    LogicalOperator::AND,
                    std::vector<std::variant<SimpleCondition, LogicalCondition> >{
                        c
                    }
                );

            token_iterator.next();
        }

        if (token_iterator.not_empty())
            logical_op = token_iterator.next();
    }

    if (current_condition.conditions.size() > 0)
        output.push(current_condition);

    LogicalCondition final_output = LogicalCondition(LogicalOperator::OR);
    while (!output.empty()) {
        if (std::holds_alternative<Parentheses>(output.top()))
            throw std::invalid_argument("Still ) in output");

        final_output.conditions.push_back(std::get<LogicalCondition>(output.top()));
        output.pop();
    }

    return final_output;
}


Column Parser::get_column(VectorIterator<Token> &it) {
    if (it.empty() || it.peek().type != Token::Type::IDENTIFIER) {
        throw std::invalid_argument("");
    }

    std::string column = it.next().value;

    if (it.empty() || (it.peek().value != "INTEGER" && it.peek().value != "TEXT")) {
        throw std::invalid_argument("Valid type (INTEGER or TEXT) must come after column name");
    }

    Token type = it.next();

    if (type.type != Token::KEYWORD || (type.value != "INTEGER" && type.value != "TEXT"))
        throw std::invalid_argument("Invalid Type");

    return Column(column, (type.value == "INTEGER") ? Column::Type::INTEGER : Column::TEXT);
}

Command Parser::get_command() {
    if (tokenized_query.size() < 1) {
        throw std::invalid_argument("Query must contain tokens.");
    }

    VectorIterator<Token> it = VectorIterator<Token>(tokenized_query.begin(), tokenized_query.end());

    std::vector<std::string> columns;
    bool all = false;

    if (it.peek().value == "SELECT") {
        it.next();
        while (it.not_empty() && it.peek().value != "FROM") {
            Token c = it.next();
            if (c.type == Token::KEYWORD) {
                if (c.value == "*") {
                    all = true;
                } else
                    throw std::invalid_argument("Keyword that is not * is not allowed here");
            } else if (c.type == Token::IDENTIFIER) {
                columns.push_back(c.value);
            }

            if (it.not_empty() && it.peek().value == ",")
                it.next();
        }

        if (!it.not_empty())
            throw std::invalid_argument("SELECT Query must contain FROM");

        it.next(); // taking out FROM

        std::string table_name = it.next().value;

        SelectCommand select_command = SelectCommand(
            columns, all, table_name
        );

        if (it.not_empty()) {
            if (it.peek().value != "WHERE") {
                throw std::invalid_argument("WHERE is the only that can come after table");
            }

            it.next(); // taking out WHERE
            select_command.where = parse_where(it);
        }

        return select_command;
    } else if (it.peek().value == "INSERT") {
        it.next();
        InsertCommand command;

        if (it.empty() || it.next().value != "INTO")
            throw std::invalid_argument("INTO must come after INSERT");

        if (it.empty())
            throw std::invalid_argument("Table name must come after INTO");

        std::string destination = it.next().value;

        command.destination = destination;

        if (it.empty() || it.next().value != "VALUES")
            throw std::invalid_argument("VALUES Must come after table name");

        if (it.empty() || it.next().value != "(")
            throw std::invalid_argument("( Must come after values");

        while (it.not_empty() && it.peek().value != ")") {
            Token c = it.next();
            if (c.type == Token::STRING_LITERAL) {
                command.values.push_back(c.value);
            } else if (c.type == Token::NUMERIC_LITERAL) {
                command.values.push_back(std::stoi(c.value));
            } else
                throw std::invalid_argument("Values must be LITERALS");

            if (it.peek().value == ",")
                it.next();
            else if (it.peek().value != ")")
                throw std::invalid_argument("INSERT query must contain , between each column definition");
        }

        return command;
    } else if (it.peek().value == "DROP") {
        DropTableCommand command;
        it.next();
        if (it.empty() || it.peek().value != "TABLE") {
            throw std::invalid_argument("TABLE keyword must come after DROP");
        }

        it.next();

        if (it.empty()) {
            throw std::invalid_argument("Table name must come after TABLE keyword.");
        }

        command.table = it.next().value;

        return command;
    } else if (it.peek().value == "CREATE") {
        it.next();
        CreateTableCommand create_table_command;

        if (it.empty() || it.peek().value != "TABLE")
            throw std::invalid_argument("TABLE Must come after CREATE");

        it.next();

        if (it.empty())
            throw std::invalid_argument("Table Name must come after TABLE");

        create_table_command.name = it.next().value;
        create_table_command.columns = std::vector<Column>();

        if (it.empty() || it.peek().value != "(") {
            throw std::invalid_argument("Query must have(<col> <type>, ...)");
        }

        it.next();

        while (it.not_empty() && it.peek().value != ")") {
            Column c = get_column(it);
            create_table_command.columns.push_back(c);
            if (it.not_empty() && it.peek().value == ",")
                it.next();
        }

        return create_table_command;
    } else if (it.peek().value == "DELETE") {
        it.next();
        if (it.empty() || it.peek().type != Token::KEYWORD || it.peek().value != "FROM") {
            throw std::invalid_argument("FROM must come after DELETE");
        }

        it.next(); // Take out FROM.

        if (it.empty() || it.peek().type != Token::IDENTIFIER)
            throw std::invalid_argument("Table name must come after FROM");

        std::string table = it.next().value;

        if (it.empty() || it.peek().type != Token::KEYWORD || it.peek().value != "WHERE")
            throw std::invalid_argument("WHERE Must come after table name");

        it.next();

        if (it.empty())
            throw std::invalid_argument("The condition must come after WHERE");

        LogicalCondition where = parse_where(it);

        return DeleteFromCommand(
            table, where
        );
    } else if (it.peek().value == "DROP") {
        it.next();
        if (it.empty() || it.peek().type != Token::KEYWORD || it.peek().value != "TABLE")
            throw std::invalid_argument("TABLE Must come after DROP");

        it.next();

        if (it.empty())
            throw std::invalid_argument("Table name must come after TABLE keyword");

        return DropTableCommand(it.peek().value);
    } else {
        throw std::invalid_argument("Not an available command.");
    }
}
