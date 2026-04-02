#include <gtest/gtest.h>
#include "../src/parser.h"
#include "../src/tokenizer.h"
#include <vector>

TEST(ParserTests, SimpleSelectTest) {
    std::string query = "SELECT a, b, c FROM table1";
    std::vector<Token> tokens = tokenize_query(query);
    Parser parser(tokens);
    Command command = parser.get_command();

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
    Command command = parser.get_command();

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
    Command command = parser.get_command();

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

    EXPECT_TRUE(
        select_command.where.has_value()
    );

    EXPECT_EQ(
        select_command.where.value().conditions.size(),
        1
    );

    EXPECT_TRUE(
        std::holds_alternative<LogicalCondition>(select_command.where.value().conditions[0])
    );

    auto simple = std::get<SimpleCondition>(
        std::get<LogicalCondition>(select_command.where.value().conditions[0]).conditions[0]);

    EXPECT_EQ(
        simple,
        SimpleCondition(
            "a", SimpleCondition::EQUALS, 2
        )
    );
}

TEST(ParserTests, WhereSelectTest_2) {
    std::string query = "SELECT a, b, c FROM table1 WHERE (a = 2 OR b = 5) OR a = 5 AND b = 7";
    std::vector<Token> tokens = tokenize_query(query);
    Parser parser(tokens);
    Command command = parser.get_command();

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

    EXPECT_TRUE(
        select_command.where.has_value()
    );

    EXPECT_EQ(
        select_command.where.value().conditions.size(),
        2
    );

    auto part1 = std::get<LogicalCondition>(select_command.where.value().conditions[0]);

    std::cout << part1 << std::endl;

    EXPECT_EQ(part1.conditions.size(), 2);
    EXPECT_EQ(part1.op, LogicalOperator::AND);

    EXPECT_EQ(
        std::get<SimpleCondition>(part1.conditions[0]),
        SimpleCondition("a", SimpleCondition::EQUALS, 5)
    );

    EXPECT_EQ(
        std::get<SimpleCondition>(part1.conditions[1]),
        SimpleCondition("b", SimpleCondition::EQUALS, 7)
    );

    auto part2 = std::get<LogicalCondition>(select_command.where.value().conditions[1]);

    std::cout << "PART 2 = " << part2 << std::endl;

    EXPECT_EQ(
        part2.conditions.size(),
        2
    );

    auto part2_1 = std::get<LogicalCondition>(part2.conditions[0]);

    EXPECT_EQ(
        part2_1.conditions.size(),
        1
    );

    EXPECT_EQ(
        std::get<SimpleCondition>(part2_1.conditions[0]),
        SimpleCondition(
            "b",
            SimpleCondition::EQUALS,
            5
        )
    );

    auto part2_2 = std::get<LogicalCondition>(part2.conditions[1]);

    EXPECT_EQ(
        part2_2.conditions.size(),
        1
    );

    EXPECT_EQ(
        std::get<SimpleCondition>(part2_2.conditions[0]),
        SimpleCondition(
            "a",
            SimpleCondition::EQUALS,
            2
        )
    );
}

TEST(ParserTests, InsertTest) {
    std::string query = "INSERT INTO t1 VALUES (123, \"word\", 523)";
    std::vector<Token> tokens = tokenize_query(query);
    Parser parser(tokens);
    Command command = parser.get_command();

    EXPECT_TRUE(
        std::holds_alternative<InsertCommand>(command)
    );

    InsertCommand insert_command = std::get<InsertCommand>(command);

    EXPECT_EQ(
        insert_command.destination,
        "t1"
    );

    EXPECT_EQ(
        insert_command.values.size(),
        3
    );

    EXPECT_EQ(
        insert_command.values[0],
        Value(123)
    );

    EXPECT_EQ(
        insert_command.values[1],
        Value("word")
    );

    EXPECT_EQ(
        insert_command.values[2],
        Value(523)
    );
}