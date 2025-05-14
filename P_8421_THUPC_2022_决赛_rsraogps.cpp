#include <cstdint> // for uint32_t
#include <cstdio>  // for fread

constexpr int N_io = 1e5 + 10;
// input
char *p1, *p2, buf[N_io];                                            // NOLINT
[[nodiscard]] inline char nc() noexcept {                            // NOLINT
  return (p1 == p2 && (p2 = (p1 = buf) + fread(buf, 1, N_io, stdin), // NOLINT
                       p1 == p2)                                     // NOLINT
              ? EOF
              : *p1++); // NOLINT
}
uint32_t read() {                    // NOLINT
  uint32_t x = 0;                    // NOLINT
  char ch = nc();                    // NOLINT
  while (ch < 48 || ch > 57)         // NOLINT
    ch = nc();                       // NOLINT
  while (ch >= 48 && ch <= 57)       // NOLINT
    x = x * 10 + ch - 48, ch = nc(); // NOLINT
  return x;                          // NOLINT
}

// output
char obuf[N_io], *p3 = obuf;                                        // NOLINT
inline void pc(char x) {                                            // NOLINT
  (p3 - obuf < N_io)                                                // NOLINT
      ? (*p3++ = x)                                                 // NOLINT
      : (fwrite(obuf, p3 - obuf, 1, stdout), p3 = obuf, *p3++ = x); // NOLINT
}
inline void write(uint32_t x) { // NOLINT
  if (!x) {                     // NOLINT
    pc('0');                    // NOLINT
    return;                     // NOLINT
  } // NOLINT
  uint32_t len = 0, k1 = x, c[40];     // NOLINT
  while (k1)                           // NOLINT
    c[len++] = k1 % 10 ^ 48, k1 /= 10; // NOLINT
  while (len--)                        // NOLINT
    pc(c[len]);                        // NOLINT
}
struct iofush { // NOLINT
  ~iofush() {
    fwrite(obuf, p3 - obuf, 1, stdout); // NOLINT
  }
} ioflush1; // NOLINT
// 以上为IO优化

#include <array>   // for array
#include <numeric> // for gcd
#include <ranges>  // for views::drop, views::iota
#include <tuple>   // for tuple
#include <vector>  // for vector

namespace {

using std::array;
using std::gcd;
using std::tuple;
using std::vector;
using std::views::drop;
using std::views::iota;
using u32 = uint32_t;

// 优化 1:使用gnu::always_inline针对短lambda函数进行内联
// 优化 2:引用传递
// 优化结果:1312ms->1012ms
auto solve() {
  // 输入
  auto arrSize = read();
  auto querySize = read();
  auto arr = vector<array<u32, 3>>(arrSize + 1);
  for (auto &i : arr | drop(1)) {
    i[0] = read();
  }
  for (auto &&i : arr | drop(1)) {
    i[1] = read();
  }
  for (auto &&i : arr | drop(1)) {
    i[2] = read();
  }
  auto lPtr = vector<u32>(querySize + 1);
  auto rPtr = vector(arrSize + 1, vector<array<u32, 2>>());
  for (u32 i : iota(1U, querySize + 1)) {
    auto l = read();
    auto r = read();
    lPtr[i] = l;
    rPtr[r].push_back({i, l});
  }
  // 对序列的操作
  auto getVal = [&arr] [[gnu::always_inline]] (u32 i) {
    return arr[i][0] * arr[i][1] * arr[i][2];
  };
  auto updArr = [&arr] [[gnu::always_inline]] (u32 i) {
    auto &[x1, y1, z1] = arr[i];
    const auto &[x2, y2, z2] = arr[i + 1];
    const auto [x3, y3, z3] = tuple(x1 & x2, y1 | y2, gcd(z1, z2));
    return x3 != x1 || y3 != y1 || z3 != z1        ? x1 = x3, y1 = y3, z1 = z3,
                                              true : false;
  };
  // 对二维前缀和的操作
  auto sum = vector(arrSize + 1, array<u32, 3>());
  auto getSum = [&sum] [[gnu::always_inline]] (const u32 i, const u32 T) {
    return sum[i][0] + (sum[i][1] * (T - sum[i][2]));
  };
  auto updSum = [&getSum, &sum, &getVal](const u32 i, const u32 T) {
    sum[i] = {
        getSum(i, T),
        sum[i - 1][1] + getVal(i),
        T,
    };
  };
  // 对答案的操作
  auto ans = vector<u32>(querySize + 1);
  auto updAns = [&ans, &getSum, &rPtr](u32 i) {
    for (auto [id, j] : rPtr[i]) {
      ans[id] = getSum(i, i) - getSum(j - 1, i);
    }
  };
  for (u32 i : iota(1U, arrSize + 1)) {
    auto k = i - 1;
    while (k != 0 && updArr(k)) {
      --k;
    }
    sum[i][0] = getSum(i - 1, i - 1);
    for (u32 j : iota(k + 1, i + 1)) {
      updSum(j, i - 1);
    }
    updAns(i);
  }
  // 输出
  for (u32 const &i : ans | drop(1)) {
    write(i), pc('\n');
  }
}
} // namespace

auto main() noexcept -> int {
  try {
    solve();
  } catch (...) {
    return 0;
  }
}