#ifndef MY_PROJECT_TOKENIZER_H
#define MY_PROJECT_TOKENIZER_H
#include <string>
#include <vector>
#include "utils.h"

class TokenizerException : public ExpectedException {
    std::string message;
public:
    explicit TokenizerException(const std::string &message) : message("[TOKENIZER] " + message) {}

    const char *what() const noexcept override {
        return message.c_str();
    }
};

std::vector<Token> tokenize_query(const std::string &query);

void print_vector(std::vector<Token> vector);

#endif
