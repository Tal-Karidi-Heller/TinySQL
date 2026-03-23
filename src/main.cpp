#include <iostream>
#include <cstdio>
#include "tokenizer.h"
#include "parser.h"
#include "engine.h"
#include <variant>
#include <cctype>

using Value = std::variant<int, std::string>;

int main() {
    Engine engine;
    while (true) {
        try {
            std::cout << "enter input: ";
            std::string input;
            std::getline(std::cin, input);
            std::vector<Token> vector = tokenize_query(input);
            print_vector(vector);

            Parser parser(vector);
            Command command = parser.get_commands();
            Status status = engine.execute_command(command);
            std::cout << status << std::endl;
            std::cout << status.output_rows.value();
        } catch (std::exception& e) {
            std::cerr << "Caught standard exception: " << e.what() << std::endl;
            std::cout << "Command Failed" << std::endl;
        }
    }
    return 0;
}
