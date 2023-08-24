#include "parser/api.h"
#include "parser/tests/printers.h"

#include <gtest/gtest.h>

#include <sstream>
#include <tuple>

using namespace regex_engine::parser;
using namespace regex_engine::tests;

namespace {

struct RepetitionFixture : public ::testing::TestWithParam<
                               std::tuple<std::string, StatusCode, std::size_t, std::string>> {};

TEST_P(RepetitionFixture, RepetitionTest) {
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
    RepetitionSuite, RepetitionFixture,
    ::testing::Values(
        std::tuple("a{3,7}", StatusCode::ACCEPTED, 6, "repetition[3,7] (charsequence ('a'))"),
        std::tuple("a{3,}", StatusCode::ACCEPTED, 5, "repetition[3,inf] (charsequence ('a'))"),
        std::tuple("a{,7}", StatusCode::NO_NUMBER, 2, ""),
        std::tuple("a{7,3}", StatusCode::INVALID_REPETITION, 5, ""),
        std::tuple("a{,}", StatusCode::NO_NUMBER, 2, ""),
        std::tuple("a{7,7}", StatusCode::ACCEPTED, 6, "repetition[7,7] (charsequence ('a'))"),
        std::tuple("a{7}", StatusCode::ACCEPTED, 4, "repetition[7,7] (charsequence ('a'))"),
        std::tuple("a{0,1}", StatusCode::ACCEPTED, 6, "zeroOrOne (charsequence ('a'))"),
        std::tuple("a{0,}", StatusCode::ACCEPTED, 5, "zeroOrMore (charsequence ('a'))"),
        std::tuple("a{1,}", StatusCode::ACCEPTED, 5, "oneOrMore (charsequence ('a'))"),
        std::tuple("(?:abc){2,4}", StatusCode::ACCEPTED, 12,
                   "repetition[2,4] (non-capturing group (charsequence ('a', 'b', 'c')))"),
        std::tuple("a+", StatusCode::ACCEPTED, 2, "oneOrMore (charsequence ('a'))"),
        std::tuple("a*", StatusCode::ACCEPTED, 2, "zeroOrMore (charsequence ('a'))"),
        std::tuple("a?", StatusCode::ACCEPTED, 2, "zeroOrOne (charsequence ('a'))"),
        std::tuple("a++", StatusCode::UNQUANTIFIABLE_EXPRESSION, 2, ""),
        std::tuple("a+*", StatusCode::UNQUANTIFIABLE_EXPRESSION, 2, ""),
        std::tuple("a+?", StatusCode::UNQUANTIFIABLE_EXPRESSION, 2, ""),
        std::tuple("a+{3,7}", StatusCode::UNQUANTIFIABLE_EXPRESSION, 2, ""),
        std::tuple("a*+", StatusCode::UNQUANTIFIABLE_EXPRESSION, 2, ""),
        std::tuple("a**", StatusCode::UNQUANTIFIABLE_EXPRESSION, 2, ""),
        std::tuple("a*?", StatusCode::UNQUANTIFIABLE_EXPRESSION, 2, ""),
        std::tuple("a*{3,7}", StatusCode::UNQUANTIFIABLE_EXPRESSION, 2, ""),
        std::tuple("a?+", StatusCode::UNQUANTIFIABLE_EXPRESSION, 2, ""),
        std::tuple("a?*", StatusCode::UNQUANTIFIABLE_EXPRESSION, 2, ""),
        std::tuple("a??", StatusCode::UNQUANTIFIABLE_EXPRESSION, 2, ""),
        std::tuple("a?{3,7}", StatusCode::UNQUANTIFIABLE_EXPRESSION, 2, ""),
        std::tuple("a{3,7}+", StatusCode::UNQUANTIFIABLE_EXPRESSION, 6, ""),
        std::tuple("a{3,7}*", StatusCode::UNQUANTIFIABLE_EXPRESSION, 6, ""),
        std::tuple("a{3,7}?", StatusCode::UNQUANTIFIABLE_EXPRESSION, 6, ""),
        std::tuple("a{3,7}{3,7}", StatusCode::UNQUANTIFIABLE_EXPRESSION, 6, "")));

} // namespace
