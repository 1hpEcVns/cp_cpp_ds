#include <cstdio>
#include <locale>

const int N_io = 1e4 + 10;

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
#include <utility> // for tuple
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
  auto const query = [querySize]() {
    auto qlr = vector<array<u32, 2>>(querySize + 1);
    auto cntR = vector(querySize + 1,0U);
    for (auto &&i : qlr | drop(1)) {
      i[0] = read(), i[1] = read(), cntR[i[1]]++;
    }
    auto res = vector(querySize + 1, vector{array{0U, 0U}});
    for (auto &&[i, vec] : enumerate(res)) {
      vec.reserve(cntR[i]);
    }
    for (auto &&[i, j] : enumerate(qlr) | drop(1)) {
      res[j[0]].push_back({static_cast<u32>(i), j[1]});
    }
    return res;
  };
  for (auto sum1 = vector<int>(arrSize + 1), sum2 = sum1;
       auto lPtr : iota(1U, querySize + 1)) {
    auto res = arr[lPtr];
    auto view = arr | drop(1) | take(lPtr) | reverse |
                take_while([&res](auto &&cur) {
                  return cur[0] &= res[0], cur[1] |= res[1],
                         cur[2] = gcd(cur[2], res[2]), res != cur ? res = cur,
                         false : true;
                }) |
                reverse;
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