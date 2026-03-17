#include <iostream>
#include "parser.h"
#include "tokenizer.h"

std::ostream &operator<<(std::ostream &os, const std::vector<std::string> &vector)
{
    for (int i = 0; i < vector.size(); i++)
    {
        std::cout << vector[i];
        if (i < vector.size() - 1)
        {
            std::cout << ", ";
        }
    }

    return os;
}

std::ostream &operator<<(std::ostream &os, const SingleCondition &condition)
{
    os << "(column = '" << condition.where_column << "', value = '" << condition.where_value << ')' << std::endl;
    return os;
}

std::ostream &operator<<(std::ostream &os, const std::vector<SingleCondition> &conditions)
{
    for (SingleCondition c : conditions)
    {
        os << c << ", ";
    }
    return os;
}

std::ostream &operator<<(std::ostream &os, const SelectCommand &command)
{
    os << "columns = " << command.columns << "(" << command.columns.size() << ")" << ", all = " << command.all << ", table_name = " << command.table_name << ", has_where_condition = "
       << command.has_where_condition << ", where_conditions = " << command.where_conditions;
    return os;
}

Parser::Parser(std::vector<Token> &tokenized_query)
{
    this->tokenized_query = tokenized_query;
}

bool Parser::skipSymbols(std::vector<Token>::iterator &it, const std::vector<Token>::iterator &end)
{
    for (; it != end && (*it).type == Token::Type::SYMBOL; it++)
    {
    }
    return it == end;
}

std::vector<SingleCondition> Parser::_get_where_conditions(std::vector<Token>::iterator &start, std::vector<Token>::iterator &end, std::vector<SingleCondition>& output)
{
    if (skipSymbols(start, end) == true)
    {
        throw std::invalid_argument("WHERE Must have Column");
    }

    SingleCondition current_condition;

    if ((*start).value == "AND")
    {
        current_condition.op = SingleCondition::Operator::AND;
        start++;

        std::cout << "WHERE condition is AND" << std::endl;

        if (skipSymbols(start, end) == true)
        {
            throw std::invalid_argument("WHERE Must have Column");
        }
    }
    else if ((*start).value == "OR")
    {
        current_condition.op = SingleCondition::Operator::OR;
        start++;

        std::cout << "WHERE condition is OR" << std::endl;

        if (skipSymbols(start, end) == true)
        {
            throw std::invalid_argument("WHERE Must have Column");
        }
    }

    current_condition.where_column = (*start).value;
    start++;

    std::cout << "WHERE column is " << current_condition.where_column << std::endl;

    if (skipSymbols(start, end) == true && (*start).value != "=")
    {
        throw std::invalid_argument("WHERE Must have =");
    }

    start++;

    if (skipSymbols(start, end) == true)
    {
        throw std::invalid_argument("WHERE Must have value.");
    }

    current_condition.where_value = (*start).value;

    std::cout << "WHERE value is " << current_condition.where_value << std::endl;

    output.push_back(current_condition);

    std::cout << "Add condition to output " << current_condition.where_value << std::endl;

    start++;

    if (skipSymbols(start, end) == false)
    {
        std::cout << "Calling function again" << std::endl;
        std::cout << "Current Token is " << (*start).value << "." << std::endl;
        _get_where_conditions(
            start,
            end,
            output);
    }

    return output;
}

std::vector<SingleCondition> Parser::get_where_conditions(std::vector<Token>::iterator start, std::vector<Token>::iterator end)
{
    std::vector<SingleCondition> output;
    return _get_where_conditions(
        start, end, output);
}

Command Parser::get_commands()
{
    if (tokenized_query[0].value == "SELECT")
    {
        // SELECT COMMAND
        std::vector<Token>::iterator it = tokenized_query.begin() + 1;
        std::vector<Token>::iterator end = tokenized_query.end();
        SelectCommand command;
        for (; it != tokenized_query.end() && (*it).value != "FROM"; it++)
        {
            Token &it_token = *it;
            if (it_token.type == Token::Type::SYMBOL)
            {
                continue;
            }

            if (it_token.type == Token::Type::KEYWORD && it_token.value == "*")
            {
                command.all = true;
            }

            else if (it_token.type == Token::Type::LITERAL)
            {
                command.columns.push_back(it_token.value);
            }
        }

        if (it == end)
        {
            throw std::invalid_argument("SELECT query must contain FROM");
        }

        it++;

        if (skipSymbols(it, end) == true)
        {
            throw std::invalid_argument("SELECT query must include table name after FROM");
        }

        command.table_name = (*it).value;

        it++;

        if (skipSymbols(it, end) == true)
        {
            std::cout << "Query does not have a where condition\n";
            return command;
        }

        std::cout << command.columns << "\n";

        // return command;

        if ((*it).value == "WHERE")
        {
            command.has_where_condition = true;
            it++;

            std::vector<SingleCondition> conditions = get_where_conditions(
                it, end);

            command.where_conditions = conditions;

            std::cout << command.where_conditions.size() << std::endl;
            std::cout << command << std::endl;

            return command;
        }
        else
        {
            throw std::invalid_argument("At the end of a SELECT query a WHERE must come.");
        }
    }
    else
    {
        throw std::invalid_argument("Not an available command.");
    }
}
