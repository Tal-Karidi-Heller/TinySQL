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

std::ostream &operator<<(std::ostream &os, const SelectCommand &command)
{
    os << "columns = " << command.columns << "(" << command.columns.size() << ")" << ", all = " << command.all << ", table_name = " << command.table_name << ", has_where_condition = "
       << command.has_where_condition << ", where_column = " << command.where_column << ", where_value = " << command.where_value;
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

            if (skipSymbols(it, end) == true)
            {
                throw std::invalid_argument("WHERE Must have Column");
            }

            command.where_column = (*it).value;
            std::cout << "where = " << command.where_column << "\n";
            it++;

            std::cout << "value = '" << (*it).value << "'.\n";

            if (skipSymbols(it, end) == true && (*it).value != "=")
            {
                throw std::invalid_argument("WHERE Must have =");
            }

            std::cout << "value = '" << (*it).value << "'.\n";

            it++;

            std::cout << "value = '" << (*it).value << "'.\n";

            if (skipSymbols(it, end) == true)
            {
                throw std::invalid_argument("WHERE Must have value.");
            }

            command.where_value = (*it).value;

            std::cout << command << "\n";

            return command;
        }
        else
        {
            throw std::invalid_argument("At the end of a SELECT query a WHERE must come.");
        }
    }
}
