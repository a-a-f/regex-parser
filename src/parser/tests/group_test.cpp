#include "parser/api.h"
#include "parser/tests/printers.h"

#include <gtest/gtest.h>

#include <sstream>
#include <tuple>

using namespace regex_engine::parser;
using namespace regex_engine::tests;

namespace {

struct GroupFixture : public ::testing::TestWithParam<
                          std::tuple<std::string, StatusCode, std::size_t, std::string>> {};

TEST_P(GroupFixture, GroupTest) {
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
    GroupSuite, GroupFixture,
    ::testing::Values(std::tuple("(?abc)", StatusCode::SYNTAX_ERROR, 2, ""),
                      std::tuple("(a(b(c)d(e)))", StatusCode::ACCEPTED, 13,
                                 "capturing group (add (charsequence ('a'), capturing group (add "
                                 "(charsequence ('b'), capturing "
                                 "group (charsequence ('c')), charsequence ('d'), capturing group "
                                 "(charsequence ('e'))))))"),
                      std::tuple("(?:a(b(?:c)d(e)))", StatusCode::ACCEPTED, 17,
                                 "non-capturing group (add (charsequence ('a'), capturing group "
                                 "(add (charsequence ('b'), "
                                 "non-capturing group (charsequence ('c')), charsequence ('d'), "
                                 "capturing group (charsequence ('e'))))))"),
                      std::tuple("(a(?:b(c)d(?:e)))", StatusCode::ACCEPTED, 17,
                                 "capturing group (add (charsequence ('a'), non-capturing group "
                                 "(add (charsequence ('b'), capturing "
                                 "group (charsequence ('c')), charsequence ('d'), non-capturing "
                                 "group (charsequence ('e'))))))")));

} // namespace
