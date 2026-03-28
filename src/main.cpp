#include <iostream>
#include <cstdio>
#include "tokenizer.h"
#include "parser.h"
// #include "engine.h"
#include <variant>

// using Value = std::variant<int, std::string>;

/*
CREATE TABLE t1 (a INTEGER, b TEXT)

INSERT INTO t1 VALUES(123, "abc")

SELECT a, b FROM t1
*/

// 1 + (1 + 5 - (2 + 3))



void test() {
    std::string a = "a = 2 AND c = 3 OR b = 5 AND (a = 5 OR b = 2 AND c = 5)";
    std::vector<Token> v = tokenize_query(a);
    VectorIterator<Token> v_it = VectorIterator<Token>(v.begin(), v.end());
    std::cout << "Started Parsing" << std::endl;
    LogicalCondition r = Parser::parse_where(v_it);
    std::cout << "Finished" << std::endl;
    std::cout << r.conditions.size() << std::endl;
    std::cout << r << std::endl;
}


int main() {
    test();
    /*
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
            // std::cout << status.output_rows.value();
        } catch (std::exception& e) {
            std::cerr << "Caught standard exception: " << e.what() << std::endl;
            std::cout << "Command Failed" << std::endl;
        }
    }
    */
    return 0;
}
