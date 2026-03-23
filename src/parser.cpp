#include <iostream>
#include "parser.h"
#include "tokenizer.h"

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

std::ostream &operator<<(std::ostream &os, const SingleCondition &condition) {
    os << "(column = '" << condition.where_column << "', value = '" << condition.where_value << "')" << std::endl;
    return os;
}

std::ostream &operator<<(std::ostream &os, const std::vector<SingleCondition> &conditions) {
    for (SingleCondition c: conditions) {
        os << c << ", ";
    }
    return os;
}

std::ostream &operator<<(std::ostream &os, const SelectCommand &command) {
    os << "columns = " << command.columns << "(" << command.columns.size() << ")" << ", all = " << command.all <<
            ", table_name = " << command.table_name << ", has_where_condition = "
            << command.has_where_condition << ", where_conditions = " << command.where_conditions;
    return os;
}

std::optional<int> convert_to_number(std::string &string) {
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

bool Parser::skipSymbols(std::vector<Token>::iterator &it, const std::vector<Token>::iterator &end) {
    for (; it != end && (*it).type == Token::Type::SYMBOL; it++) {
    }
    return it == end;
}


std::vector<SingleCondition> Parser::_get_where_conditions(std::vector<Token>::iterator &start,
                                                           std::vector<Token>::iterator &end,
                                                           std::vector<SingleCondition> &output) {
    if (start == end) {
        throw std::invalid_argument("WHERE Must have Column");
    }

    SingleCondition current_condition;

    if (start->value == "AND") {
        current_condition.op = SingleCondition::Operator::AND;
        ++start;

        std::cout << "WHERE condition is AND" << std::endl;

        if (start == end) {
            throw std::invalid_argument("WHERE Must have Column");
        }
    } else if (start->value == "OR") {
        current_condition.op = SingleCondition::Operator::OR;
        ++start;

        std::cout << "WHERE condition is OR" << std::endl;

        if (start == end) {
            throw std::invalid_argument("WHERE Must have Column");
        }
    }

    current_condition.where_column = start->value;
    ++start;

    std::cout << "WHERE column is " << current_condition.where_column << std::endl;

    if (start == end || start->value != "=") {
        throw std::invalid_argument("WHERE Must have =");
    }

    ++start;

    if (start == end) {
        throw std::invalid_argument("WHERE Must have value.");
    }

    if (start->type != Token::STRING_LITERAL && start->type != Token::NUMERIC_LITERAL)
        throw std::invalid_argument("Condition value must be literal -> Number or string literal");

    if (start->type == Token::STRING_LITERAL)
        current_condition.where_value = start->value;
    else
        current_condition.where_value = std::stoi(start->value);

    std::cout << "WHERE value is " << current_condition.where_value << std::endl;
    std::cout << "WHERE VALUE IS NUMBER " << std::holds_alternative<int>(current_condition.where_value) << std::endl;

    output.push_back(current_condition);

    std::cout << "Add condition to output " << current_condition.where_value << std::endl;

    ++start;

    if (start != end) {
        std::cout << "Calling function again" << std::endl;
        std::cout << "Current Token is " << (*start).value << "." << std::endl;
        _get_where_conditions(
            start,
            end,
            output);
    }

    return output;
}

std::vector<SingleCondition> Parser::get_where_conditions(std::vector<Token>::iterator start,
                                                          std::vector<Token>::iterator end) {
    std::vector<SingleCondition> output;
    return _get_where_conditions(
        start, end, output);
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
    if (tokenized_query[0].value == "SELECT") {
        // SELECT COMMAND
        std::vector<Token>::iterator it = tokenized_query.begin() + 1;
        std::vector<Token>::iterator end = tokenized_query.end();
        SelectCommand command;
        for (; it != tokenized_query.end() && (*it).value != "FROM"; it++) {
            Token &it_token = *it;

            if (it_token.type == Token::Type::KEYWORD && it_token.value == "*") {
                command.all = true;
            } else if (it_token.type == Token::Type::IDENTIFIER) {
                command.columns.push_back(it_token.value);
                if ((*(it + 1)).value == ",") {
                    it++;
                } else if ((*(it + 1)).value != "FROM") {
                    throw std::invalid_argument("SELECT query must contain , between each column");
                }
            }
        }

        if (it == end) {
            throw std::invalid_argument("SELECT query must contain FROM");
        }

        it++;

        if (it == end) {
            throw std::invalid_argument("SELECT query must include table name after FROM");
        }

        command.table_name = (*it).value;

        it++;

        if (it == end) {
            std::cout << "Query does not have a where condition\n";
            return command;
        }

        std::cout << command.columns << "\n";

        // return command;

        if ((*it).value == "WHERE") {
            command.has_where_condition = true;
            it++;

            std::vector<SingleCondition> conditions = get_where_conditions(
                it, end);

            command.where_conditions = conditions;

            std::cout << command.where_conditions.size() << std::endl;
            std::cout << command << std::endl;

            return command;
        } else {
            throw std::invalid_argument("At the end of a SELECT query a WHERE must come.");
        }
    } else if (tokenized_query[0].value == "INSERT") {
        std::cout << "INSERT parsing" << std::endl;
        std::vector<Token>::iterator it = tokenized_query.begin() + 1;
        std::vector<Token>::iterator end = tokenized_query.end();

        InsertCommand command;

        if (it == end || (*it).value != "INTO") {
            throw std::invalid_argument("Into must come after INSERT.");
        }

        it++;

        if (it == end) {
            throw std::invalid_argument("Name must come after INTO.");
        }

        command.destination = it->value;
        std::cout << "destination " << command.destination << std::endl;

        it++;

        if (it == end || it->value != "VALUES") {
            throw std::invalid_argument("VALUES Must come after table name");
        }

        it++;

        if (it == end || it->value != "(") {
            throw std::invalid_argument("( Must come after VALUES");
        }
        it++;

        std::cout << it->value << std::endl;
        for (; it != end && it->value != ")"; it++) {
            Token c_token = *it;
            std::cout << "c_token = " << c_token.value << std::endl;
            if (c_token.type == Token::STRING_LITERAL)
                command.values.push_back(c_token.value);
            else if (c_token.type == Token::NUMERIC_LITERAL)
                command.values.push_back(std::stoi(c_token.value));
            else
                throw std::invalid_argument("Values must be LITERALS");

            if ((it + 1)->value == ",") {
                ++it;
            } else if ((it + 1)->value != ")") {
                throw std::invalid_argument("SELECT query must contain , between each column");
            }
        }

        std::cout << "Finished" << std::endl;

        // std::cout << command.values << std::endl;

        return command;
    } else if (tokenized_query[0].value == "DELETE") {
        std::vector<Token>::iterator it = tokenized_query.begin() + 1;
        std::vector<Token>::iterator end = tokenized_query.end();
        DeleteFromCommand command;

        if ((*it).value != "FROM") {
            throw std::invalid_argument("FROM Must after come after DELETE");
        }

        it++;

        if (it == end) {
            throw std::invalid_argument("Name must come after from FROM in DELETE SQL.");
        }

        command.table = it->value;
        it++;

        if (it == end || it->value != "WHERE") {
            throw std::invalid_argument("WHERE Must come after table name in DELETE FROM");
        }

        it++;

        command.where_condition = get_where_conditions(it, end);

        std::cout << command.where_condition << std::endl;

        return command;
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
