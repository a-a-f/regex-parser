#include "parser/api.h"
#include "parser/tests/printers.h"

#include <gtest/gtest.h>

#include <sstream>
#include <tuple>

using namespace regex_engine::parser;
using namespace regex_engine::tests;

namespace {

struct RegexFixture1
    : public ::testing::TestWithParam<std::tuple<std::string, StatusCode, std::size_t>> {};

TEST_P(RegexFixture1, RegexTest1) {
  auto [regex, code, lastPos] = GetParam();
  auto result = parseRegex(regex);

  EXPECT_EQ(result.code, code);
  EXPECT_EQ(result.lastPos, lastPos);
  EXPECT_EQ(static_cast<bool>(result.node), result.code == StatusCode::ACCEPTED);
}

INSTANTIATE_TEST_SUITE_P(
    RegexSuite1, RegexFixture1,
    ::testing::Values(
        std::tuple("ab|c", StatusCode::ACCEPTED, 4), std::tuple("a", StatusCode::ACCEPTED, 1),
        std::tuple("abc", StatusCode::ACCEPTED, 3), std::tuple("a|(b|c)", StatusCode::ACCEPTED, 7),
        std::tuple("(a|b)|c", StatusCode::ACCEPTED, 7),
        std::tuple("(", StatusCode::PARENTHESIS_IMBALANCE, 1),
        std::tuple(")", StatusCode::SYNTAX_ERROR, 0), std::tuple("a|b|c", StatusCode::ACCEPTED, 5),
        std::tuple("a|b|c|d", StatusCode::ACCEPTED, 7),
        std::tuple("a|(b|c)|d", StatusCode::ACCEPTED, 9),
        std::tuple("a+b+c+d+", StatusCode::ACCEPTED, 8),
        std::tuple("(ab+)?.*", StatusCode::ACCEPTED, 8),
        std::tuple("ab++", StatusCode::UNQUANTIFIABLE_EXPRESSION, 3),
        std::tuple("(ab)++", StatusCode::UNQUANTIFIABLE_EXPRESSION, 5),
        std::tuple("()", StatusCode::SYNTAX_ERROR, 1),
        std::tuple(")(", StatusCode::SYNTAX_ERROR, 0), std::tuple("*", StatusCode::SYNTAX_ERROR, 0),
        std::tuple("a(", StatusCode::PARENTHESIS_IMBALANCE, 2),
        std::tuple("a**", StatusCode::UNQUANTIFIABLE_EXPRESSION, 2),
        std::tuple("", StatusCode::SYNTAX_ERROR, 0),
        std::tuple("((ab)+", StatusCode::PARENTHESIS_IMBALANCE, 6),
        std::tuple("(ab))+", StatusCode::SYNTAX_ERROR, 4),
        std::tuple("(ab)+)", StatusCode::SYNTAX_ERROR, 5),
        std::tuple("a||b", StatusCode::SYNTAX_ERROR, 2),
        std::tuple("a|b|", StatusCode::SYNTAX_ERROR, 4),
        std::tuple("a(bb)+a", StatusCode::ACCEPTED, 7),
        std::tuple("a.(bb)+a?", StatusCode::ACCEPTED, 9),
        std::tuple("a.(b|(c+|d+))+a?", StatusCode::ACCEPTED, 16),
        std::tuple(".", StatusCode::ACCEPTED, 1), std::tuple(".+", StatusCode::ACCEPTED, 2)));

struct RegexFixture2 : public ::testing::TestWithParam<
                           std::tuple<std::string, StatusCode, std::size_t, std::string>> {};

TEST_P(RegexFixture2, RegexTest2) {
  auto [regex, code, lastPos, expected] = GetParam();
  auto result = parseRegex(regex);

  EXPECT_EQ(result.code, code);
  EXPECT_EQ(result.lastPos, lastPos);

  if (result.code == StatusCode::ACCEPTED) {
    ASSERT_TRUE(result.node);
    auto ostr = std::ostringstream();
    print(ostr, result.node);
    EXPECT_EQ(ostr.str(), expected);
  } else {
    EXPECT_FALSE(result.node);
  }
}

INSTANTIATE_TEST_SUITE_P(
    RegexSuite2, RegexFixture2,
    ::testing::Values(
        std::tuple("ab*", StatusCode::ACCEPTED, 3,
                   "add (charsequence ('a'), zeroOrMore (charsequence ('b')))"),
        std::tuple("(ab)*", StatusCode::ACCEPTED, 5,
                   "zeroOrMore (capturing group (charsequence ('a', 'b')))"),
        std::tuple("ab|a", StatusCode::ACCEPTED, 4,
                   "orr (charsequence ('a', 'b'), charsequence ('a'))"),
        std::tuple("a(b|a)", StatusCode::ACCEPTED, 6,
                   "add (charsequence ('a'), capturing group (orr (charsequence ('b'), "
                   "charsequence ('a'))))"),
        std::tuple("a|b|c", StatusCode::ACCEPTED, 5,
                   "orr (charsequence ('a'), charsequence ('b'), charsequence ('c'))"),
        std::tuple("a|b*", StatusCode::ACCEPTED, 4,
                   "orr (charsequence ('a'), zeroOrMore (charsequence ('b')))"),
        std::tuple("(a|b)*", StatusCode::ACCEPTED, 6,
                   "zeroOrMore (capturing group (orr (charsequence ('a'), charsequence ('b'))))"),
        std::tuple("a", StatusCode::ACCEPTED, 1, "charsequence ('a')"),
        std::tuple("ab", StatusCode::ACCEPTED, 2, "charsequence ('a', 'b')"),
        std::tuple("a.*", StatusCode::ACCEPTED, 3, "add (charsequence ('a'), zeroOrMore (any ()))"),
        std::tuple("(a.*)|(bb)", StatusCode::ACCEPTED, 10,
                   "orr (capturing group (add (charsequence ('a'), zeroOrMore (any ()))), "
                   "capturing group (charsequence ('b', 'b')))"),
        std::tuple("abcdefg", StatusCode::ACCEPTED, 7,
                   "charsequence ('a', 'b', 'c', 'd', 'e', 'f', 'g')"),
        std::tuple("\\+", StatusCode::ACCEPTED, 2, "charsequence ('+')"),
        std::tuple("a\\+*", StatusCode::ACCEPTED, 4,
                   "add (charsequence ('a'), zeroOrMore (charsequence ('+')))"),
        std::tuple(
            "ab\\d?", StatusCode::ACCEPTED, 5,
            "add (charsequence ('a', 'b'), zeroOrOne (charclass (include [[0, 9]] exclude [])))")));

} // namespace
