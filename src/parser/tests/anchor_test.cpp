#include "parser/api.h"
#include "parser/tests/printers.h"

#include <gtest/gtest.h>

#include <sstream>
#include <tuple>

using namespace regex_engine::parser;
using namespace regex_engine::tests;

namespace {

struct AnchorFixture : public ::testing::TestWithParam<
                           std::tuple<std::string, StatusCode, std::size_t, std::string>> {};

TEST_P(AnchorFixture, AnchorTest) {
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
    AnchorSuite, AnchorFixture,
    ::testing::Values(
        std::tuple("^abc$", StatusCode::ACCEPTED, 5,
                   "add (anchor{line start}, charsequence ('a', 'b', 'c'), anchor{line end})"),
        std::tuple(
            "^^ab$^c$$^^^$", StatusCode::ACCEPTED, 13,
            "add (anchor{line start}, anchor{line start}, charsequence ('a', 'b'), anchor{line "
            "end}, anchor{line start}, charsequence ('c'), anchor{line end}, anchor{line end}, "
            "anchor{line start}, anchor{line start}, anchor{line start}, anchor{line end})"),
        std::tuple("^+", StatusCode::UNQUANTIFIABLE_EXPRESSION, 1, ""),
        std::tuple("$+", StatusCode::UNQUANTIFIABLE_EXPRESSION, 1, ""),
        std::tuple("(^)+", StatusCode::ACCEPTED, 4,
                   "oneOrMore (capturing group (anchor{line start}))"),
        std::tuple("($)+", StatusCode::ACCEPTED, 4,
                   "oneOrMore (capturing group (anchor{line end}))"),
        std::tuple("\\Aabc\\Z", StatusCode::ACCEPTED, 7,
                   "add (anchor{string start}, charsequence ('a', 'b', 'c'), anchor{string end})"),
        std::tuple("\\A+", StatusCode::UNQUANTIFIABLE_EXPRESSION, 2, ""),
        std::tuple("\\Z+", StatusCode::UNQUANTIFIABLE_EXPRESSION, 2, ""),
        std::tuple("(\\A)+", StatusCode::ACCEPTED, 5,
                   "oneOrMore (capturing group (anchor{string start}))"),
        std::tuple("(\\Z)+", StatusCode::ACCEPTED, 5,
                   "oneOrMore (capturing group (anchor{string end}))")));

} // namespace
