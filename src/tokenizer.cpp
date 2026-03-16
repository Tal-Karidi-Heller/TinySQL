#include <iostream>
#include "tokenizer.h"

int add(int a, int b)
{
    return a + b;
}

const std::string keyword_arguments[] = {
    "SELECT",
    "CREATE",
    "INSERT",
    "INTO",
    "DELETE",
    "FROM",
    "DROP",
    "TABLE",
    "*"};

const std::string operators[] = {
    "="};

bool is_keyword(std::string element)
{
    for (const std::string &keyword : keyword_arguments)
    {
        if (keyword == element)
        {
            return true;
        }
    }

    return false;
}

bool is_operator(std::string &element)
{
    for (const std::string &op : operators)
    {
        if (op == element)
        {
            return true;
        }
    }
    return false;
}

bool indicating_identifier(std::string lastValue)
{
    if (lastValue == "FROM" || lastValue == "INTO" || lastValue == "TABLE")
    {
        return true;
    }

    return false;
}

std::vector<Token> tokenize_query(std::string &query)
{
    std::vector<Token> output;

    Token currentToken;
    currentToken.value = "";
    currentToken.type = Token::Type::UNDEFINED;

    std::cout << "Starting\n";

    for (char &c : query)
    {

        // std::cout << "c = '" << c << '\'' << '\n';
        if (c != ',' && c != ' ' && c != '(' && c != ')')
        {
            // std::cout << "c is not symbol" << '\n';
            currentToken.value += c;
        }
        else
        {
            // std::cout << "c is a symbol" << '\n';
            // Finished the current token.
            // Segmenting the current token.
            if (!currentToken.value.empty())
            {
                if (is_keyword(currentToken.value))
                {
                    currentToken.type = Token::Type::KEYWORD;
                }
                else if (output.size() > 1 && indicating_identifier(output[output.size() - 1].value) == true)
                {
                    currentToken.type = Token::Type::IDENTIFIER;
                }
                else if(is_operator(currentToken.value)) {
                    currentToken.type = Token::Type::OPERATOR;
                }
                else
                {
                    currentToken.type = Token::Type::LITERAL;
                }

                output.push_back(currentToken);
            }

            currentToken.value = c;
            currentToken.type = Token::Type::SYMBOL;

            // std::cout << "added '" << currentToken.value << "'\n";

            output.push_back(currentToken);

            currentToken.value = "";
            currentToken.type = Token::Type::SYMBOL;
        }
    }

    std::cout << currentToken.value << "\n";

    if (!currentToken.value.empty())
    {
        if (is_keyword(currentToken.value))
        {
            currentToken.type = Token::Type::KEYWORD;
        }
        else if (output.size() > 1 && indicating_identifier(output[output.size() - 1].value) == true)
        {
            currentToken.type = Token::Type::IDENTIFIER;
        }
        else
        {
            currentToken.type = Token::Type::LITERAL;
        }
        std::cout << "pushing " << currentToken.value;
        output.push_back(currentToken);
    }

    return output;
}

std::ostream &operator<<(std::ostream &os, const Token::Type &type)
{
    switch (type)
    {
    case Token::KEYWORD:
        os << "KEYWORD";
        break;
    case Token::IDENTIFIER:
        os << "IDENTIFIER";
        break;
    case Token::LITERAL:
        os << "LITERAL";
        break;
    case Token::OPERATOR:
        os << "OPERATOR";
        break;
    case Token::SYMBOL:
        os << "SYMBOL";
        break;
    case Token::UNDEFINED:
        os << "UNDEFINED";
        break;
    }
    return os;
}

void print_vector(std::vector<Token> vector)
{
    for (int i = 0; i < vector.size(); i++)
    {
        Token currentToken = vector[i];
        std::cout << "value = '" << currentToken.value << "', type = " << currentToken.type;
        if (i < vector.size() - 1)
        {
            std::cout << "\n";
        }
        else
        {
            std::cout << '\n';
        }
    }
}