//
// Created by Tal Karidi on 02/04/2026.
//

#include <gtest/gtest.h>
#include <vector>
#include "../src/parser.h"
#include "../src/tokenizer.h"
#include "../src/engine.h"

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

TEST(EntireProject_Tests, CreateAndSelect) {
    std::vector<std::string> queries = {
        "CREATE TABLE t1 (a INTEGER, b INTEGER, c TEXT)",
        "INSERT INTO t1 VALUES (2, 3, \"1\")",
        "INSERT INTO t1 VALUES (4, 5, \"2\")",
        "INSERT INTO t1 VALUES (4, 5, \"2.5\")",
        "INSERT INTO t1 VALUES (7, 8, \"3\")",
        "select a, b, c FROM t1"
    };

    std::vector<Status> outputs = execute_queries(queries);

    auto &rows = outputs[outputs.size() - 1].output_rows.value();

    EXPECT_EQ(
        rows.size(),
        4
    );

    EXPECT_EQ(
        rows[0],
        (
            std::vector<Value> {
            Value(2), Value(3), Value("1")
            }
        )
    );

    EXPECT_EQ(
        rows[1],
        (
            std::vector<Value> {
            Value(4), Value(5), Value("2")
            }
        )
    );

    EXPECT_EQ(
        rows[2],
        (
            std::vector<Value> {
            Value(4), Value(5), Value("2.5")
            }
        )
    );

    EXPECT_EQ(
        rows[3],
        (
            std::vector<Value> {
            Value(7), Value(8), Value("3")
            }
        )
    );
}

TEST(EntireProject_Tests, CreateAndDelete) {
    std::vector<std::string> queries = {
        "CREATE TABLE t1 (a INTEGER, b INTEGER, c TEXT)",
        "INSERT INTO t1 VALUES (2, 3, \"1\")",
        "INSERT INTO t1 VALUES (4, 5, \"2\")",
        "INSERT INTO t1 VALUES (4, 5, \"2.5\")",
        "INSERT INTO t1 VALUES (7, 8, \"3\")",
        "DELETE FROM t1 WHERE a = 4 OR b = 3",
        "SELECT * FROM t1"
    };

    std::vector<Status> outputs = execute_queries(queries);

    Status delete_output = outputs[outputs.size() - 2];

    EXPECT_EQ(
        delete_output.output_rows.value().size(),
        3
    );

    EXPECT_EQ(
        delete_output.output_rows.value()[0],
        (
            std::vector<Value> {
                2, 3, "1"
            }
        )
    );

    EXPECT_EQ(
        delete_output.output_rows.value()[1],
        (
            std::vector<Value> {
                4, 5, "2"
            }
        )
    );

    EXPECT_EQ(
        delete_output.output_rows.value()[2],
        (
            std::vector<Value> {
                4, 5, "2.5"
            }
        )
    );

    Status select_output = outputs[outputs.size() - 1];

    EXPECT_EQ(
        select_output.output_rows.value().size(),
        1
    );

    EXPECT_EQ(
        select_output.output_rows.value()[0],
        (
            std::vector<Value> {
                7, 8, "3"
            }
        )
    );
}

TEST(EntireProject_Tests, Invalid_WhereClause) {
    std::vector<std::string> queries = {
        "CREATE TABLE t1 (a INTEGER, b INTEGER, c TEXT)",
        "INSERT INTO t1 VALUES (2, 3, \"1\")",
        "INSERT INTO t1 VALUES (4, 5, \"2\")",
    };

    Engine engine;
    execute_queries(queries, engine);

    Parser parser = Parser(
        tokenize_query(
            "SELECT * FROM t1 where a = 2 AND c = 1"
        )
    );

    EXPECT_THROW(
        engine.execute_command(
            parser.get_command()
        ),
        EngineException
    );
}
