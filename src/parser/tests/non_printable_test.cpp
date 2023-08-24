#include "parser/api.h"
#include "parser/tests/printers.h"

#include <gtest/gtest.h>

#include <sstream>
#include <tuple>

using namespace regex_engine::parser;
using namespace regex_engine::tests;

namespace {

struct NonPrintableFixture : public ::testing::TestWithParam<
                                 std::tuple<std::string, StatusCode, std::size_t, std::string>> {};

TEST_P(NonPrintableFixture, NonPrintableTest) {
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
    NonPrintableSuite, NonPrintableFixture,
    ::testing::Values(
        std::tuple("\\t", StatusCode::ACCEPTED, 2, "charsequence ('$9')"),
        std::tuple("\\r", StatusCode::ACCEPTED, 2, "charsequence ('$d')"),
        std::tuple("\\n", StatusCode::ACCEPTED, 2, "charsequence ('$a')"),
        std::tuple("\\a", StatusCode::ACCEPTED, 2, "charsequence ('\a')"),
        std::tuple("\\e", StatusCode::ACCEPTED, 2, "charsequence ('" + std::string(1, 0x1b) + "')"),
        std::tuple("\\f", StatusCode::ACCEPTED, 2, "charsequence ('$c')"),
        std::tuple("\\v", StatusCode::ACCEPTED, 2, "charsequence ('$b')"),
        std::tuple("\\cI", StatusCode::ACCEPTED, 3, "charsequence ('$9')"),
        std::tuple("\\x", StatusCode::ACCEPTED, 2, "charsequence ('" + std::string(1, 0x00) + "')"),
        std::tuple("\\x41", StatusCode::ACCEPTED, 4, "charsequence ('A')"),
        std::tuple("\\xA", StatusCode::ACCEPTED, 3, "charsequence ('$a')"),
        std::tuple("\\x0A", StatusCode::ACCEPTED, 4, "charsequence ('$a')"),
        std::tuple("\\o{101}", StatusCode::ACCEPTED, 7, "charsequence ('A')"),
        std::tuple("\\o{0101}", StatusCode::ACCEPTED, 8, "charsequence ('A')"),
        std::tuple("\\o{00101}", StatusCode::ACCEPTED, 9, "charsequence ('A')")));

} // namespace
