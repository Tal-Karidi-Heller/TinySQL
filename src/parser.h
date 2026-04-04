#ifndef PARSER_H
#define PARSER_H
#include <vector>
#include <variant>
#include "tokenizer.h"
#include "table.h"
#include "utils.h"
#include "commands.h"

class ParsingException : public ExpectedException {
    std::string message;
public:
    explicit ParsingException(const std::string &message) : message("[PARSER] " + message) {}

    const char* what() const noexcept override {
        return message.c_str();
    }
};

class Parser {
private:
    static Column get_column(VectorIterator<Token> &it);

public:
    std::vector<Token> tokenized_query;

    Parser(const std::vector<Token> &tokenized_query);

    Command get_command();

    static LogicalCondition parse_where(VectorIterator<Token> &token_iterator);
};

#endif
