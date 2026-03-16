#ifndef TOKENIZER_H
#define TOKENIZER_H
#include <string>
#include <vector>

struct Token {
    enum Type {KEYWORD, IDENTIFIER, LITERAL, SYMBOL, UNDEFINED, OPERATOR};
    Type type;
    std::string value;
};

std::vector<Token> tokenize_query(std::string& query);

void print_vector(std::vector<Token> vector);

#endif