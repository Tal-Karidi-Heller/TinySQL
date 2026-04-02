//
// Created by Tal Karidi on 01/04/2026.
//

#include <gtest/gtest.h>
#include "../src/parser.h"
#include "../src/tokenizer.h"
#include <vector>
#include "../src/engine.h"

TEST(EngineTests, CreateAndInsert) {
    Engine engine;

    engine.execute_command(
        CreateTableCommand(
            "t1",
            std::vector<Column>{
                Column("a", Column::Type::INTEGER),
                Column("b", Column::Type::TEXT),
                Column("c", Column::Type::INTEGER)
            }
        )
    );

    engine.execute_command(
        InsertCommand(
            "t1",
            std::vector<Value>{
                Value(1),
                Value("abc"),
                Value(10)
            }
        )
    );

    engine.execute_command(
        InsertCommand(
            "t1",
            std::vector<Value>{
                Value(2),
                Value("abc"),
                Value(10)
            }
        )
    );

    Status response = engine.execute_command(
        SelectCommand(
            std::vector<std::string>{
                "a",
                "b",
                "c"
            },
            false,
            "t1"
        )
    );

    EXPECT_TRUE(
        response.output_rows.has_value()
    );

    EXPECT_EQ(
        response.output_rows.value().size(),
        2
    );

    auto _1 = std::vector<Value>{
        Value(1),
        Value("abc"),
        Value(10)
    };

    EXPECT_EQ(
        response.output_rows.value()[0],
        _1
    );

    auto _2 = std::vector<Value>{
        Value(2),
        Value("abc"),
        Value(10)
    };

    EXPECT_EQ(
        response.output_rows.value()[1],
        _2
    );
}

TEST(EngineTests, SelectWhere) {
    Engine engine;

    engine.execute_command(
        CreateTableCommand(
            "t1",
            std::vector<Column>{
                Column("a", Column::Type::INTEGER),
                Column("b", Column::Type::TEXT),
                Column("c", Column::Type::INTEGER)
            }
        )
    );

    engine.execute_command(
        InsertCommand(
            "t1",
            std::vector<Value>{
                Value(1),
                Value("abc"),
                Value(10)
            }
        )
    );

    engine.execute_command(
        InsertCommand(
            "t1",
            std::vector<Value>{
                Value(2),
                Value("abc"),
                Value(10)
            }
        )
    );

    engine.execute_command(
        InsertCommand(
            "t1",
            std::vector<Value>{
                Value(5),
                Value("abcd"),
                Value(20)
            }
        )
    );

    engine.execute_command(
        InsertCommand(
            "t1",
            std::vector<Value>{
                Value(5),
                Value("abcd"),
                Value(30)
            }
        )
    );

    SelectCommand command = SelectCommand(
        std::vector<std::string>{
            "a",
            "b",
            "c"
        },
        false,
        "t1"
    );

    // WHERE (b = "abc" OR c = 20) AND a = 1 OR a = 5

    command.where = LogicalCondition(
        LogicalOperator::AND,
        std::vector<std::variant<SimpleCondition, LogicalCondition> >{
            LogicalCondition(
                LogicalOperator::OR,
                std::vector<std::variant<SimpleCondition, LogicalCondition> >{
                    SimpleCondition(
                        "b",
                        SimpleCondition::EQUALS,
                        "abc"
                    ),
                    SimpleCondition(
                        "c",
                        SimpleCondition::EQUALS,
                        20
                    )
                }
            ),

            LogicalCondition(
                LogicalOperator::OR,
                std::vector<std::variant<SimpleCondition, LogicalCondition>> {
                    SimpleCondition(
                        "a",
                        SimpleCondition::EqualOp::EQUALS,
                        1
                    ),
                    SimpleCondition(
                        "a",
                        SimpleCondition::EqualOp::EQUALS,
                        5
                    )
                }
            )
        }
    );

    Status response = engine.execute_command(
        command
    );

    EXPECT_TRUE(
        response.output_rows.has_value()
    );

    EXPECT_EQ(
        response.output_rows.value().size(),
        2
    );

    auto _1 = std::vector<Value>{
        Value(1),
        Value("abc"),
        Value(10)
    };

    EXPECT_EQ(
        response.output_rows.value()[0],
        _1
    );

    auto _2 = std::vector<Value>{
        Value(5),
        Value("abcd"),
        Value(20)
    };

    EXPECT_EQ(
        response.output_rows.value()[1],
        _2
    );
}
