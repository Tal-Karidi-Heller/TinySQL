#ifndef TOKENIZER_H
#define TOKENIZER_H
#include <string>
#include <vector>
#include <iostream>

struct Token {
    enum Type {KEYWORD, IDENTIFIER, STRING_LITERAL, NUMERIC_LITERAL, SYMBOL, UNDEFINED, OPERATOR};
    Type type;
    std::string value;

    Token() : type(Type::UNDEFINED), value(std::string("")) {}
    Token(const std::string &value, const Type type) : type(type), value(value) {}
};

inline std::ostream& operator<<(std::ostream &os, const Token::Type &t) {
    switch (t) {
        case Token::KEYWORD: return os << "KEYWORD";
        case Token::IDENTIFIER: return os << "IDENTIFIER";
        case Token::STRING_LITERAL: return os << "STRING_LITERAL";
        case Token::NUMERIC_LITERAL: return os << "NUMERIC_LITERAL";
        case Token::SYMBOL: return os << "SYMBOL";
        case Token::UNDEFINED: return os << "UNDEFINED";
        case Token::OPERATOR: return os << "OPERATOR";
    }
    return os;
}

inline std::ostream& operator<<(std::ostream &os, const Token &t) {
    os << "[Token] {type = " << t.type << ", value = " << t.value << "}";
    return os;
}

std::vector<Token> tokenize_query(const std::string& query);

void print_vector(std::vector<Token> vector);

#endif