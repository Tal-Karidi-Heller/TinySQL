#include <gtest/gtest.h>
#include "../src/parser.h"
#include "../src/tokenizer.h"
#include <vector>

#define EXPECT_TOKEN(a, b) \
    EXPECT_EQ(a.value, b.value); \
    EXPECT_EQ(a.type, b.type);


TEST(TokenizerTests, SelectTest) {
    std::string query = "SELECT a, b, c FROM table1";
    std::vector<Token> vector = tokenize_query(
        query
    );

    EXPECT_TOKEN(vector[0], Token("SELECT", Token::KEYWORD));

    EXPECT_TOKEN(vector[1], Token("a", Token::IDENTIFIER));
    EXPECT_TOKEN(vector[2], Token(",", Token::SYMBOL));

    EXPECT_TOKEN(vector[3], Token("b", Token::IDENTIFIER));
    EXPECT_TOKEN(vector[4], Token(",", Token::SYMBOL));

    EXPECT_TOKEN(vector[5], Token("c", Token::IDENTIFIER));

    EXPECT_TOKEN(vector[6], Token("FROM", Token::Type::KEYWORD));

    EXPECT_TOKEN(vector[7], Token("table1", Token::Type::IDENTIFIER));
}

TEST(TokenizerTests, LowerCase_Select) {
    std::string query = "select a, b, c from table1";
    std::vector<Token> vector = tokenize_query(
        query
    );

    EXPECT_TOKEN(vector[0], Token("SELECT", Token::KEYWORD));

    EXPECT_TOKEN(vector[1], Token("a", Token::IDENTIFIER));
    EXPECT_TOKEN(vector[2], Token(",", Token::SYMBOL));

    EXPECT_TOKEN(vector[3], Token("b", Token::IDENTIFIER));
    EXPECT_TOKEN(vector[4], Token(",", Token::SYMBOL));

    EXPECT_TOKEN(vector[5], Token("c", Token::IDENTIFIER));

    EXPECT_TOKEN(vector[6], Token("FROM", Token::KEYWORD));

    EXPECT_TOKEN(vector[7], Token("table1", Token::IDENTIFIER));
}