#include <iostream> 
#include <cstdio>
#include "tokenizer.h"

int main() {
    std::string input;
    std::getline(std::cin, input);
    std::vector<Token> vector = tokenize_query(input);
    print_vector(vector);
    return 0;
}