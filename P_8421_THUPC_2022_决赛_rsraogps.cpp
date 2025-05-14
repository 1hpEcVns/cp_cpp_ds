#include <cstdio>
#include <locale>

const int N_io = 1e4 + 10;

char *p1, *p2, buf[N_io]; //
NOLINT
[[nodiscard]] inline char nc() noexcept { //
  NOLINT
  return (p1 == p2 && (p2 = (p1 = buf) + fread(buf, 1, N_io, stdin), //
                       NOLINT p1 == p2)                              //
                  NOLINT
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
// 以上为快读快写

#include <array>
#include <numeric> // for gcd
#include <ranges>  // for views
#include <tuple>   // for tuple
#include <vector>  // for vector

namespace {

using std::array;
using std::gcd;
using std::tuple;
using std::vector;
using std::views::drop;
using std::views::enumerate;
using std::views::iota;
using std::views::reverse;
using std::views::take;
using std::views::take_while;
using std::views::zip;
using u32 = uint32_t;

auto solve() {
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
  auto rPtr = vector(querySize + 1, vector<array<u32, 2>>());
  for (u32 i : iota(1U, querySize + 1)) {
    lPtr[i] = read();
    rPtr[lPtr[i]].push_back({i, read()});
  }
  auto ans = vector<u32>(querySize + 1);
  auto sum = vector(arrSize + 1, array<u32, 3>());
  auto getVal = [&arr](u32 i) { return arr[i][0] * arr[i][1] * arr[i][2]; };
  auto getSum = [&sum](u32 i, u32 T) {
    return sum[i][0] + (sum[i][1] * (T - sum[i][2]));
  };
  auto updSum = [&getSum, &sum, &getVal](u32 i, u32 T) {
    sum[i] = {
        getSum(i, T),
        sum[i - 1][1] + getVal(i),
        T,
    };
  };
  for (u32 i : iota(1U, arrSize + 1)) {
    auto j = i - 1;
    for (; j != 0; j--) {
      auto &[x1, y1, z1] = arr[j];
      const auto &[x2, y2, z2] = arr[j + 1];
      const auto [x3, y3, z3] = tuple(x1 & x2, y1 | y2, gcd(z1, z2));
      if (x3 == x1 && y3 == y1 && z3 == z1) {
        break;
      }
      x1 = x3, y1 = y3, z1 = z3;
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