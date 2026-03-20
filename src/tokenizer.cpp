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

const char symbols[] = {
    ',', '(', ')'};

bool
is_keyword(std::string element)
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

bool is_symbol(char& c) {
    for (const char& symbol : symbols) {
        if (c == symbol) {
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

Token::Type classify_token(std::string string, std::vector<Token> classified_tokens)
{
    Token::Type type = Token::Type::UNDEFINED;
    if (is_keyword(string))
    {
        type = Token::Type::KEYWORD;
    }
    else if (classified_tokens.size() > 1 && indicating_identifier(classified_tokens[classified_tokens.size() - 1].value) == true)
    {
        type = Token::Type::IDENTIFIER;
    }
    else if (is_operator(string))
    {
        type = Token::Type::OPERATOR;
    }
    else
    {
        type = Token::Type::LITERAL;
    }

    return type;
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
        if (c != ' ' && is_symbol(c) == false)
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
                currentToken.type = classify_token(currentToken.value, output);
                output.push_back(currentToken);
            }

            if (c != ' ')
            {
                currentToken.value = c;
                currentToken.type = Token::Type::SYMBOL;
                output.push_back(currentToken);
            }

            currentToken.value = "";
            currentToken.type = Token::Type::UNDEFINED;
        }
    }

    std::cout << currentToken.value << "\n";

    if (!currentToken.value.empty())
    {
        currentToken.type = classify_token(currentToken.value, output);
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