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
namespace {
auto solve() {
  auto const n = read();
  auto const blockSize = static_cast<int>(sqrt(n));
  auto queryAmount = read();
  auto a = vector(n + 1, array<int, 4>{});
  auto const update = [&a, blockSize, n](int const &x) {
    auto &[y, nxt, lst, sum] = a[x];
    if (y > n) {
      nxt = y, lst = x, sum = 1;
    } else if (x / blockSize == y / blockSize && y <= n) { // 同一块且不越界
      nxt = a[y][1], lst = a[y][2], sum = a[y][3] + 1;
    } else {
      nxt = y, lst = y, sum = 1;
    }
  };
  auto const query = [&a, n](int const &pos) {
    auto res = 0;
    auto cur_pos = pos;
    auto lst_pos = pos;
    while (cur_pos <= n) {
      auto const &[o, nxt, lst, sum] = a[cur_pos];
      cur_pos = nxt, lst_pos = lst, res += sum;
    }
    write(lst_pos), pc(' '), write(res), pc('\n');
  };
  for (auto &pos : a | drop(1)) {
    pos[0] = read();
  }
  for (auto const pos : iota(1, n + 1) | reverse) {
    a[pos][0] += pos, update(pos);
  }

  while ((queryAmount--) != 0) {
    auto const opt = read();
    auto const pos = read();
    if (opt == 1) {
      query(pos);
    } else {
      a[pos][0] = pos + read();
      for (auto const cur_pos :
           iota(pos / blockSize * blockSize, pos + 1) | reverse) {
        update(cur_pos);
      }
    }
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