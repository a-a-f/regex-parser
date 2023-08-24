#include "parser/api.h"
#include "parser/tests/printers.h"

#include <gtest/gtest.h>

#include <sstream>
#include <tuple>

using namespace regex_engine::parser;
using namespace regex_engine::tests;

namespace {

struct CharClassFixture : public ::testing::TestWithParam<
                              std::tuple<std::string, StatusCode, std::size_t, std::string>> {};

TEST_P(CharClassFixture, CharClassTest) {
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
    CharClassSuite, CharClassFixture,
    ::testing::Values(
        std::tuple("[a-z]", StatusCode::ACCEPTED, 5, "charclass (include [[a, z]] exclude [])"),
        std::tuple("[]-z]", StatusCode::ACCEPTED, 5, "charclass (include [[], z]] exclude [])"),
        std::tuple("[abc]", StatusCode::ACCEPTED, 5, "charclass (include [[a, c]] exclude [])"),
        std::tuple("[a-z0-9]", StatusCode::ACCEPTED, 8,
                   "charclass (include [[0, 9][a, z]] exclude [])"),
        std::tuple("[abc", StatusCode::BRACKET_IMBALANCE, 4, ""),
        std::tuple("[[:digit:]]", StatusCode::ACCEPTED, 11,
                   "charclass (include [[0, 9]] exclude [])"),
        std::tuple("[[:^digit:]]", StatusCode::ACCEPTED, 12,
                   "charclass (include [] exclude [[0, 9]])"),
        std::tuple("[[:dixit:]]", StatusCode::INVALID_POSIX_CLASS, 9, ""),
        std::tuple("[[:digit:]0-9abcA-Z]", StatusCode::ACCEPTED, 20,
                   "charclass (include [[0, 9][A, Z][a, c]] exclude [])"),
        std::tuple("[a-fd-s]", StatusCode::ACCEPTED, 8, "charclass (include [[a, s]] exclude [])"),
        std::tuple("[0a-c12e-g34i-k567m-o89q-s]", StatusCode::ACCEPTED, 27,
                   "charclass (include [[0, 9][a, c][e, g][i, k][m, o][q, s]] exclude [])"),
        std::tuple("[[:digit:]]+", StatusCode::ACCEPTED, 12,
                   "oneOrMore (charclass (include [[0, 9]] exclude []))"),
        std::tuple("[]", StatusCode::BRACKET_IMBALANCE, 2, ""),
        std::tuple("[][]", StatusCode::ACCEPTED, 4,
                   "charclass (include [[[, [][], ]]] exclude [])"),
        std::tuple("[]a[]", StatusCode::ACCEPTED, 5,
                   "charclass (include [[[, [][], ]][a, a]] exclude [])"),
        std::tuple("[][a]", StatusCode::ACCEPTED, 5,
                   "charclass (include [[[, [][], ]][a, a]] exclude [])"),
        std::tuple("[]]", StatusCode::ACCEPTED, 3, "charclass (include [[], ]]] exclude [])"),
        std::tuple("[[]", StatusCode::ACCEPTED, 3, "charclass (include [[[, []] exclude [])"),
        std::tuple("[a][b]", StatusCode::ACCEPTED, 6,
                   "add (charclass (include [[a, a]] exclude []), charclass (include [[b, b]] "
                   "exclude []))"),
        std::tuple("[][][][]", StatusCode::ACCEPTED, 8,
                   "add (charclass (include [[[, [][], ]]] exclude []), charclass (include [[[, "
                   "[][], ]]] exclude []))"),
        std::tuple("[a][]", StatusCode::BRACKET_IMBALANCE, 5, ""),
        std::tuple("[a][]b]", StatusCode::ACCEPTED, 7,
                   "add (charclass (include [[a, a]] exclude []), charclass (include [[], ]][b, "
                   "b]] exclude []))"),
        std::tuple("[[]]", StatusCode::ACCEPTED, 4,
                   "add (charclass (include [[[, []] exclude []), charsequence (']'))"),
        std::tuple("]", StatusCode::ACCEPTED, 1, "charsequence (']')"),
        std::tuple("[[:a:b]", StatusCode::ACCEPTED, 7,
                   "charclass (include [[:, :][[, [][a, b]] exclude [])"),
        std::tuple("[[:a:]", StatusCode::INVALID_POSIX_CLASS, 5, ""),
        std::tuple("[[:a:b]", StatusCode::ACCEPTED, 7,
                   "charclass (include [[:, :][[, [][a, b]] exclude [])"),
        std::tuple("[[:a:b:]", StatusCode::ACCEPTED, 8,
                   "charclass (include [[:, :][[, [][a, b]] exclude [])"),
        std::tuple("[[:a[:digit:]]", StatusCode::ACCEPTED, 14,
                   "charclass (include [[0, :][[, [][a, a]] exclude [])"),
        std::tuple("[\\[-\\]]", StatusCode::ACCEPTED, 7, "charclass (include [[[, ]]] exclude [])"),
        std::tuple("[[-]]", StatusCode::ACCEPTED, 5,
                   "add (charclass (include [[-, -][[, []] exclude []), charsequence (']'))"),
        std::tuple("[#-\\]]", StatusCode::ACCEPTED, 6, "charclass (include [[#, ]]] exclude [])"),
        std::tuple("[a-]]", StatusCode::ACCEPTED, 5,
                   "add (charclass (include [[-, -][a, a]] exclude []), charsequence (']'))"),
        std::tuple("[a-b--c]", StatusCode::ACCEPTED, 8, "charclass (include [[-, c]] exclude [])"),
        std::tuple("[a-b-d]", StatusCode::ACCEPTED, 7,
                   "charclass (include [[-, -][a, b][d, d]] exclude [])"),
        std::tuple("[a\\W123]", StatusCode::ACCEPTED, 8,
                   "charclass (include [[1, 3][a, a]] exclude [[0, 9][A, Z][_, _][a, z]])"),
        std::tuple("[^abc]", StatusCode::ACCEPTED, 6, "charclass (include [] exclude [[a, c]])"),
        std::tuple("[^a\\W123]", StatusCode::ACCEPTED, 9,
                   "charclass (include [[0, 9][A, Z][_, _][a, z]] exclude [[1, 3][a, a]])"),
        std::tuple("[[:digit:]-b]", StatusCode::SYNTAX_ERROR, 10, ""),
        std::tuple("[#-[:digit:]]", StatusCode::INVALID_BOUNDS, 11, ""),
        std::tuple("[#-[:digit]", StatusCode::ACCEPTED, 11,
                   "charclass (include [[#, [][d, d][g, g][i, i][t, t]] exclude [])"),
        std::tuple("[\\w-b]", StatusCode::SYNTAX_ERROR, 3, ""),
        std::tuple("[b-\\w]", StatusCode::INVALID_BOUNDS, 4, ""),
        std::tuple("[\\o{101}b]", StatusCode::ACCEPTED, 10,
                   "charclass (include [[A, A][b, b]] exclude [])"),
        std::tuple("[\\o{101}-b]", StatusCode::ACCEPTED, 11,
                   "charclass (include [[-, -][A, A][b, b]] exclude [])"),
        std::tuple("[b-\\o{101}]", StatusCode::ACCEPTED, 11,
                   "charclass (include [[-, -][A, A][b, b]] exclude [])")));

} // namespace
