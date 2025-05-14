#include <cstdint>
#include <cstdio>

constexpr int N_io = 1e5 + 1;

char *p1, *p2, buf[N_io];                 // NOLINT
[[nodiscard]] inline char nc() noexcept { // NOLINT

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

#include <algorithm>
#include <array>
#include <bit>
#include <ranges>
#include <utility>
#include <vector>

// 本题分为三个部分
// RMQ 问题
// 莫队(离线)
namespace {
using std::array;
using std::bit_width;
using std::max;
using std::min;
using std::vector;
using std::ranges::sort;
using std::ranges::unique;
using std::views::drop;
using std::views::filter;
using std::views::iota;
using std::views::zip;
using namespace std; // NOLINT
auto solve() {
  // 读入a数组并建立a_id映射
  auto arrSize = read();
  auto qrySize = read();
  auto a = vector(arrSize + 1, 0);
  auto cnt = vector(arrSize + 1, 0);
  for (int &i : a | drop(1)) {
    i = read(), ++cnt[i];
  }
  auto aId = vector(arrSize + 1, vector<int>());
  for (int i : iota(1, arrSize + 1)) {
    aId[i].reserve(cnt[i]), cnt[i] = 0;
  }
  for (int i : iota(1, arrSize + 1)) {
    aId[a[i]].push_back(i);
  }
  // ST表 begin
  const int stSize = bit_width(static_cast<uint32_t>(arrSize));
  auto st = vector(stSize, vector<array<int, 2>>());
  // 更新minmax信息
  auto minmaxUpd = [](const array<int, 2> &a, const array<int, 2> &b) {
    return array{min(a[0], b[0]), max(a[1], b[1])};
  };
  // 读入排列y_k
  for (auto i : iota(1, arrSize)) {
    int x = read();
    st[0][i] = array{x, x};
  }
  // 初始化ST表
  for (int i : iota(1, stSize)) {
    st[i].resize(arrSize - (1 << i) + 1);
    for (int j : iota(1, arrSize - (1 << i) + 1)) {
      st[i][j] = minmaxUpd(st[i - 1][j], st[i - 1][j + (1 << (i - 1))]);
    }
  }
  // 通过st表查询y_k的[l,r]区间的minmax信息
  auto stQry = [&](int const l, int const r) {
    int k = bit_width(static_cast<uint32_t>(r - l + 1)) - 1;
    return minmaxUpd(st[k][l], st[k][r - (1 << k) + 1]);
  };
  // ST表 end
  // 莫队 begin
  auto lPtr = vector(qrySize + 1, 0);
  auto rPtr = vector(qrySize + 1, 0);
  for (auto &&[l, r] : zip(lPtr, rPtr) | drop(1)) {
    l = read(), r = read(), ++cnt[l];
  }
  auto lId = vector(qrySize + 1, vector<int>());
  for (int i : iota(1, qrySize + 1)) {
    lId[i].reserve(cnt[i]);
  }
  for (int i : iota(1, qrySize + 1)) {
    lId[lPtr[i]].push_back(i);
  }

  // 莫队 end

  // 立方根分块前缀和 begin
  auto sum1 = vector(arrSize + 1, 0LL);
  auto sum2 = vector((arrSize >> 6) + 1, 0LL);
  auto sum3 = vector((arrSize >> 12) + 1, 0LL);
  auto sumUpd = [&sum1, &sum2, &sum3](int const i, int64_t const x) {
    sum1[i] += x, sum2[i >> 6] += x, sum3[i >> 12] += x;
  };
  auto sumQry = [&](int const i) {
    auto res = 0LL;
    for (auto k : iota((i >> 6) << 6, i + 1)) {
      res += sum1[k];
    }
    for (auto k : iota((i >> 12) << 6, i >> 6)) {
      res += sum2[k];
    }
    for (auto k : iota(0, i >> 12)) {
      res += sum3[k];
    }
    return res;
  };
  // 立方根分块前缀和 end
}
} // namespace

auto main() noexcept -> int {
  try {
    solve();
  } catch (...) {
    return 0;
  }
}