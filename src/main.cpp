#include <iostream>
#include <cstdio>
#include "tokenizer.h"
#include "parser.h"
#include "utils.h"
#include "engine.h"
#include <variant>

std::vector<Status> execute_queries(std::vector<std::string> queries, Engine &engine) {
    std::vector<Status> outputs;

    for (int i = 0; i < queries.size(); i++) {
        std::vector<Token> t = tokenize_query(queries[i]);
        Parser parser = Parser(t);
        Command command = parser.get_command();
        outputs.push_back(engine.execute_command(command));
    }

    return outputs;
}

std::vector<Status> execute_queries(std::vector<std::string> queries) {
    Engine engine;
    return execute_queries(queries, engine);
}

void test() {
    std::vector<std::string> queries {
        "CREATE TABLE t1 (a INTEGER, b INTEGER, c TEXT)",
        "INSERT INTO t1 VALUES (2, 3, \"1\")",
        "INSERT INTO t1 VALUES (4, 5, \"2\")",
        "INSERT INTO t1 VALUES (4, 5, \"2.5\")",

        "CREATE TABLE t2 (word TEXT, number INTEGER)",
        "INSERT INTO t2 VALUES (\"abc\", 3)",
        "INSERT INTO t2 VALUES (\"word2\", 4)",
    };

    Engine engine;
    execute_queries(queries, engine);
    std::ofstream file("tables.txt");
    engine.save_to_file(file);
}


int main() {
    Engine engine;
    std::ifstream file = std::ifstream("/Users/tal/Desktop/TinySql/build/tables.txt");
    engine.load_from_file(file);
    std::cout << "SUCESS" << std::endl;
    std::cout << engine.list_tables() << std::endl;
    execute_queries(
        std::vector<std::string> {
            "SELECT * FROM t1",
            "SELECT * FROM t2"
        },
        engine
    );
    return 0;
    while (true) {
        try {
            std::cout << "Enter Input: ";
            std::string input;
            std::getline(std::cin, input);

            if (input == ".tables")
                std::cout << engine.list_tables() << std::endl;
            else if (input == ".quit") {
                std::ofstream file = std::ofstream("tables.txt");
                engine.save_to_file(
                    file
                );
                break;
            }
            else {
                std::vector<Token> vector = tokenize_query(input);
                print_vector(vector);

                Parser parser(vector);
                Command command = parser.get_command();
                Status status = engine.execute_command(command);
            }
        } catch (std::exception& e) {
            std::cerr << "Caught standard exception: " << e.what() << std::endl;
            std::cout << "Command Failed" << std::endl;
        }
    }
    return 0;
}
