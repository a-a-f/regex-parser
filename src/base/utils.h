#pragma once

#include <algorithm>
#include <concepts>
#include <limits>
#include <utility>
#include <vector>

namespace regex_engine::base {

template <std::integral T> using Ranges = std::vector<std::pair<T, T>>;

//
// Merge overlapping or adjacent ranges.
//
template <std::integral T> auto mergeRanges(Ranges<T>& ranges) -> void {
  if (ranges.empty()) {
    return;
  }

  std::ranges::sort(
      ranges, [](const auto& lhs, const auto& rhs) noexcept { return lhs.first < rhs.first; });

  auto it = ranges.cbegin();
  auto output = Ranges<T>();

  output.push_back(*it);
  for (++it; it != ranges.cend(); ++it) {
    const auto& entry = *it;
    auto& back = output.back();

    if (auto delta = back.second < std::numeric_limits<T>::max() ? 1 : 0;
        entry.first <= back.second + delta) {
      if (entry.second > back.second) {
        back.second = entry.second;
      }
    } else {
      output.emplace_back(entry.first, entry.second);
    }
  }

  ranges = std::move(output);
}

} // namespace regex_engine::base
