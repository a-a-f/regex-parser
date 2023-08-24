#include "base/utils.h"

#include <gtest/gtest.h>
#include <limits>

using namespace regex_engine::base;

namespace {

TEST(UtilsTest, MergeRangesEmpty) {
  auto ranges = Ranges<int>{};
  auto expected = ranges;

  mergeRanges(ranges);

  ASSERT_EQ(ranges, expected);
}

TEST(UtilsTest, MergeRangesOneRange) {
  auto ranges = Ranges<int>{{1, 2}};
  auto expected = ranges;

  mergeRanges(ranges);

  ASSERT_EQ(ranges, expected);
}

TEST(UtilsTest, MergeRangesTwoDisjointRanges) {
  auto ranges = Ranges<int>{{1, 10}, {16, 30}};
  auto expected = ranges;

  mergeRanges(ranges);

  ASSERT_EQ(ranges, expected);
}

TEST(UtilsTest, MergeRangesTwoOverlappingRanges) {
  auto ranges = Ranges<int>{{1, 10}, {7, 30}};
  auto expected = Ranges<int>{{1, 30}};

  mergeRanges(ranges);

  ASSERT_EQ(ranges, expected);
}

TEST(UtilsTest, MergeRangesTwoAdjacentRanges) {
  auto ranges = Ranges<int>{{1, 7}, {8, 30}};
  auto expected = Ranges<int>{{1, 30}};

  mergeRanges(ranges);

  ASSERT_EQ(ranges, expected);
}

TEST(UtilsTest, MergeRangesVariousRanges) {
  auto ranges = Ranges<int>{{1, 10}, {7, 30}, {31, 40}, {50, 70}, {55, 100}};
  auto expected = Ranges<int>{{1, 40}, {50, 100}};

  mergeRanges(ranges);

  ASSERT_EQ(ranges, expected);
}

TEST(UtilsTest, MergeRangesUnsortedRanges) {
  auto ranges = Ranges<int>{{50, 70}, {1, 10}, {7, 30}, {55, 100}, {31, 40}};
  auto expected = Ranges<int>{{1, 40}, {50, 100}};

  mergeRanges(ranges);

  ASSERT_EQ(ranges, expected);
}

TEST(UtilsTest, MergeRangesDoesNotOverflow) {
  auto ranges = Ranges<int>{{1, std::numeric_limits<int>::max()}, {2, 7}};
  auto expected = Ranges<int>{{1, std::numeric_limits<int>::max()}};

  mergeRanges(ranges);

  ASSERT_EQ(ranges, expected);
}

} // namespace
