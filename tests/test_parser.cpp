#include <gtest/gtest.h>
#include "../src/parser.h"
#include "../src/tokenizer.h"
#include <vector>

TEST(ParserTests, SimpleSelectTest) { 
    std::string query = "SELECT a, b, c FROM table1";
    std::vector<Token> tokens = tokenize_query(query);
    Parser parser(tokens);
    Command command = parser.get_commands();
    
    EXPECT_TRUE(
        std::holds_alternative<SelectCommand>(command)
    );

    SelectCommand select_command = std::get<SelectCommand>(command);

    EXPECT_EQ(
        select_command.table_name,
        "table1"
    );

    EXPECT_EQ(
        select_command.columns.size(),
        3
    );

    std::string columns[] = {"a", "b", "c"};

    for (int i = 0; i < select_command.columns.size(); i++) {
        EXPECT_EQ(
            columns[i],
            select_command.columns[i]
        );
    }
}

TEST(ParserTests, SelectAllTest) { 
    std::string query = "SELECT * FROM table1";
    std::vector<Token> tokens = tokenize_query(query);
    Parser parser(tokens);
    Command command = parser.get_commands();
    
    EXPECT_TRUE(
        std::holds_alternative<SelectCommand>(command)
    );

    SelectCommand select_command = std::get<SelectCommand>(command);

    EXPECT_EQ(
        select_command.table_name,
        "table1"
    );

    EXPECT_EQ(
        select_command.columns.size(),
        0
    );

    EXPECT_EQ(
        select_command.all,
        true
    );
}

TEST(ParserTests, WhereSelectTest) { 
    std::string query = "SELECT a, b, c FROM table1 WHERE a = 2";
    std::vector<Token> tokens = tokenize_query(query);
    Parser parser(tokens);
    Command command = parser.get_commands();
    
    EXPECT_TRUE(
        std::holds_alternative<SelectCommand>(command)
    );

    SelectCommand select_command = std::get<SelectCommand>(command);

    EXPECT_EQ(
        select_command.table_name,
        "table1"
    );

    EXPECT_EQ(
        select_command.columns.size(),
        3
    );

    std::string columns[] = {"a", "b", "c"};

    for (int i = 0; i < select_command.columns.size(); i++) {
        EXPECT_EQ(
            columns[i],
            select_command.columns[i]
        );
    }

    EXPECT_EQ(
        select_command.has_where_condition,
        true
    );

    EXPECT_EQ(
        select_command.where_column,
        "a"
    );

    EXPECT_EQ(
        select_command.where_value,
        "2"
    );
}