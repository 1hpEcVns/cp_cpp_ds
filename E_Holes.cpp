#include <cstdio>

const int N_io = 1e4 + 10;

char *p1, *p2, buf[N_io];                                            // NOLINT
[[nodiscard]] inline char nc() noexcept {                            // NOLINT
  return (p1 == p2 && (p2 = (p1 = buf) + fread(buf, 1, N_io, stdin), // NOLINT
                       p1 == p2)                                     // NOLINT
              ? EOF
              : *p1++); // NOLINT
}
int read() {                         // NOLINT
  int x = 0;                         // NOLINT
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
inline void write(int x) { // NOLINT
  if (!x) {                // NOLINT
    pc('0');               // NOLINT
    return;                // NOLINT
  } // NOLINT
  int len = 0, k1 = x, c[40];          // NOLINT
  if (k1 < 0)                          // NOLINT
    k1 = -k1, pc('-');                 // NOLINT
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
#include <cmath>
#include <ranges>
#include <vector>

using std::array;
using std::sqrt;
using std::vector;
using std::views::drop;
using std::views::iota;
using std::views::reverse;

auto solve() {
  auto const n = read(), B = static_cast<int>(sqrt(n));
  auto m = read();
  auto a = vector(n + 1, array<int, 4>{});
  for (auto &i : a | drop(1)) {
    i[0] = read();
  }
  auto const update = [&a, B, n](int const &x) {
    auto &[y, nxt, lst, sum] = a[x];
    if (y > n) {
      nxt = y, lst = x, sum = 1;
    } else if (x / B == y / B && y <= n) { // 同一块且不越界
      nxt = a[y][1], lst = a[y][2], sum = a[y][3] + 1;
    } else {
      nxt = y, lst = y, sum = 1;
    }
  };
  for (auto const i : iota(1, n + 1) | reverse) {
    a[i][0] += i, update(i);
  }
  auto const query = [&a, n](int x) {
    auto res = 0, y = x;
    while (x <= n) {
      auto const &[o, nxt, lst, sum] = a[x];
      x = nxt, y = lst, res += sum;
    }
    write(y), pc(' '), write(res), pc('\n');
  };
  while (m--) {
    auto const op = read(), x = read();
    if (op) {
      query(x);
    } else {
      for (a[x][0] = x + read();
           auto const i : iota(x / B * B, x + 1) | reverse) {
        a[i][0] += i, update(i);
      }
    }
  }
}

auto main() noexcept -> int {
  try {
    solve();
  } catch (...) {
    return 1;
  }
}