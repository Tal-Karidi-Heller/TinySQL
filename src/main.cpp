#include <iostream>
#include "tokenizer.h"
#include "parser.h"
#include "engine.h"
#include <variant>
#include <filesystem>

int main() {
    Engine engine;

    if (std::filesystem::exists("tables.db")) {
        std::ifstream file("tables.db");
        std::cout << "Found a data-save. using tables.db" << std::endl;
        engine.load_from_file(
            file
        );
    }

    while (true) {
        try {
            std::cout << "Enter Input: ";
            std::string input;
            std::getline(std::cin, input);

            if (input == ".tables")
                std::cout << engine.list_tables() << std::endl;
            else if (input == ".quit") {
                std::ofstream file = std::ofstream("tables.db");
                engine.save_to_file(
                    file
                );
                break;
            } else {
                std::vector<Token> vector = tokenize_query(input);

                Parser parser(vector);
                Command command = parser.get_command();
                Status status = engine.execute_command(command);
            }
        } catch (ExpectedException &e) {
            std::cerr << "Caught expected exception: " << e.what() << std::endl;
        } catch (std::exception &e) {
            std::cerr << "Caught un-expected exception: " << e.what() << std::endl;
            std::cout << "Command Failed" << std::endl;
        }
    }
    return 0;
}
