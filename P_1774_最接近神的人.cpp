#include <cmath> //for exp,log gcc需要 clang msvc不需要

#include <algorithm> //for sort
#include <array>     //for array
#include <iostream>  //for cin,cout
#include <ranges>    //for iota
#include <vector>    //for vector

namespace {
using std::array;
using std::cin;
using std::cout;
using std::greater;
using std::vector;
using std::ranges::sort;
using std::views::drop;
using std::views::iota;
auto solve() {
  cin.tie(nullptr)->sync_with_stdio(false);
  int arrSize;
  cin >> arrSize;
  auto arr = vector(arrSize + 1, array{0, 0});
  for (auto i : iota(1, arrSize + 1)) {
    cin >> arr[i][0];
    arr[i][1] = i;
  }
  auto arr1 = vector(arrSize, 0);
  auto arr2 = vector((arrSize >> 6) + 1, 0);
  auto arr3 = vector((arrSize >> 12) + 1, 0);
  auto qry = [&arr1, &arr2, &arr3](int x) {
    int64_t ans = 0;
    for (auto i : iota((x >> 6) << 6, x + 1)) {
      ans += arr1[i];
    }
    for (auto i : iota((x >> 12) << 6, x >> 6)) {
      ans += arr2[i];
    }
    for (auto i : iota(0, x >> 12)) {
      ans += arr3[i];
    }
    return ans;
  };
  auto upd = [&arr1, &arr2, &arr3](int x) {
    arr1[x]++, arr2[x >> 6]++, arr3[x >> 12]++;
  };
  sort(arr | drop(1), greater{});
  int64_t ans = 0;
  for (auto &[x, y] : arr | drop(1)) {
    ans += qry(y - 1);
    upd(y);
  }
  cout << ans;
}
} // namespace

auto main() noexcept -> int {
  try {
    solve();
  } catch (...) {
    return 0;
  }
}