#ifndef TOKENIZER_H
#define TOKENIZER_H
#include <string>
#include <vector>

struct Token {
    enum Type {KEYWORD, IDENTIFIER, STRING_LITERAL, NUMERIC_LITERAL, SYMBOL, UNDEFINED, OPERATOR};
    Type type;
    std::string value;

    Token() : type(Type::UNDEFINED), value(std::string("")) {}
    Token(const std::string &value, Type type) : type(type), value(value) {}
};

std::vector<Token> tokenize_query(std::string& query);

void print_vector(std::vector<Token> vector);

#endif