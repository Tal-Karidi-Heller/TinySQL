#include <iostream>
#include "tokenizer.h"

const std::string keyword_arguments[] = {
    "SELECT",
    "CREATE",
    "INSERT",
    "INTO",
    "DELETE",
    "FROM",
    "DROP",
    "TABLE",
    "WHERE",
    "INTEGER",
    "TEXT",
    "*"
};

const std::string operators[] = {
    "="
};

const char symbols[] = {
    ',', '(', ')'
};

std::string str_toupper(std::string s)
{
    std::transform(s.begin(), s.end(), s.begin(),
                   [](unsigned char c){ return std::toupper(c); }
                  );
    return s;
}

bool is_keyword(std::string element) {
    std::string upper_ = str_toupper(element);
    for (const std::string &keyword: keyword_arguments)
        if (keyword == upper_)
            return true;
    return false;
}

bool is_operator(std::string &element) {
    for (const std::string &op: operators)
        if (op == element)
            return true;
    return false;
}

bool is_symbol(const char &c) {
    for (const char &symbol: symbols)
        if (c == symbol)
            return true;
    return false;
}

bool indicating_identifier(std::string lastValue) {
    if (lastValue == "FROM" || lastValue == "INTO" || lastValue == "TABLE")
        return true;

    return false;
}

bool is_number_literal(std::string string) {
    for (char &c: string) {
        if (!std::isdigit(c))
            return false;
    }
    return true;
}

Token::Type classify_token(std::string string, std::vector<Token> classified_tokens) {
    Token::Type type = Token::Type::UNDEFINED;
    if (is_keyword(string))
        type = Token::Type::KEYWORD;
    else if (is_number_literal(string)) {
        type = Token::NUMERIC_LITERAL;
    } else if (is_operator(string)) {
        type = Token::OPERATOR;
    } else
        type = Token::Type::IDENTIFIER;

    return type;
}

std::vector<Token> tokenize_query(const std::string &query) {
    std::vector<Token> output;

    Token currentToken;
    currentToken.value = "";
    currentToken.type = Token::Type::UNDEFINED;


    for (const char &c: query) {
        if (c == '"' || c == '\'') {
            if (currentToken.value.size() == 0) {
                // It means we are starting a new string.
                currentToken.type = Token::STRING_LITERAL;
            } else {
                if (currentToken.type != Token::STRING_LITERAL)
                    throw std::invalid_argument("\" is not matching");
                else {
                    output.push_back(currentToken);
                    currentToken.value = "";
                    currentToken.type = Token::UNDEFINED;
                }
            }
        } else if ((currentToken.type == Token::STRING_LITERAL) || (c != ' ' && is_symbol(c) == false)) {
            currentToken.value += c;
        } else {
            // Finished the current token.
            // Segmenting the current token.
            if (!currentToken.value.empty()) {
                currentToken.type = classify_token(currentToken.value, output);
                output.push_back(currentToken);
            }

            if (c != ' ') {
                currentToken.value = c;
                currentToken.type = Token::Type::SYMBOL;
                output.push_back(currentToken);
            }

            currentToken.value = "";
            currentToken.type = Token::Type::UNDEFINED;
        }
    }

    if (!currentToken.value.empty()) {
        currentToken.type = classify_token(currentToken.value, output);
        output.push_back(currentToken);
    }

    for (Token &t: output) {
        if (t.type == Token::KEYWORD)
           t.value = str_toupper(t.value);
    }

    return output;
}

void print_vector(std::vector<Token> vector) {
    for (int i = 0; i < vector.size(); i++) {
        Token currentToken = vector[i];
        std::cout << "value = '" << currentToken.value << "', type = " << currentToken.type;
        if (i < vector.size() - 1) {
            std::cout << "\n";
        } else {
            std::cout << '\n';
        }
    }
}
