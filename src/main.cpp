#include <iostream> 
#include <cstdio>
#include "tokenizer.h"
#include "parser.h"

int main() {
    std::string input;
    std::getline(std::cin, input);
    std::vector<Token> vector = tokenize_query(input);
    print_vector(vector);
    
    Parser parser(vector);    
    parser.get_commands();
    return 0;
}