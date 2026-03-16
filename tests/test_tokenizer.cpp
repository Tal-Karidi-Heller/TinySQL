#include <gtest/gtest.h>
#include "../src/tokenizer.h"
#include <vector>

TEST(TokenizerTests, SelectTest) {
    std::vector<Token> vector = tokenize_query(
        "SELECT a, b, c FROM table1"
    );

    EXPECT_EQ(vector[0].value, "SELECT");
    EXPECT_EQ(vector[0].type, Token::Type::KEYWORD);

    EXPECT_EQ(vector[1].value, " ");
    EXPECT_EQ(vector[1].type, Token::Type::SYMBOL);

    std::cout << "vecotor[1] = '" << vector[1].value << '\'';

    std::array<std::string, 3> select_columns = {"a", "b", "c"};
    int position = 2;
    int index = 0;

    for (std::string column : select_columns) {
        EXPECT_EQ(
            vector[position].value,
            column
        );
        
        EXPECT_EQ(
            vector[position].type,
            Token::Type::LITERAL
        );

        if (index < select_columns.size() - 1) {
            position += 1;

            EXPECT_EQ(
                vector[position].value,
                ","
            );
            
            EXPECT_EQ(
                vector[position].type,
                Token::Type::SYMBOL
            );
        }

        position += 1;

        EXPECT_EQ(
            vector[position].value,
            " "
        );
        
        EXPECT_EQ(
            vector[position].type,
            Token::Type::SYMBOL
        );

        position += 1;
        index += 1;
    }
}