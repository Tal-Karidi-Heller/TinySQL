#include <iostream> 
#include <cstdio>
#include "tokenizer.h"
#include "parser.h"


int main() {
    std::string input;
    std::getline(std::cin, input);
    std::vector<Token> vector = tokenize_query(input);
    std::vector<Token>::iterator start = vector.begin();
    std::vector<Token>::iterator start2 = start;
    
    std::cout << "\n";
    std::cout << &((*start).value) << std::endl;
    std::cout << &((*start2).value) << std::endl;
    std::cout << std::endl;
    std::cout << &start << std::endl;
    std::cout << &start2 << std::endl;
    print_vector(vector);
    
    Parser parser(vector);    
    Command command = parser.get_commands();
    if (std::holds_alternative<SelectCommand>(command)) {
        SelectCommand command_ = std::get<SelectCommand>(command);
        std::cout << command_ << std::endl;
    }
    return 0;
}