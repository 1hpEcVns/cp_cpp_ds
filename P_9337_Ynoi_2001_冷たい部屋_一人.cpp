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
inline void write(long long x) { // NOLINT
  if (!x) {                      // NOLINT
    pc('0');                     // NOLINT
    return;                      // NOLINT
  } // NOLINT
  int len = 0, k1 = x, c[40];          // NOLINT
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
#include <cmath>

#include <algorithm>
#include <array>
#include <bit>
#include <ranges>
#include <stack>
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
using std::stack;
using std::vector;
using std::ranges::sort;
using std::ranges::to;
using std::views::chunk;
using std::views::drop;
using std::views::filter;
using std::views::iota;
using std::views::reverse;
using std::views::zip;
constexpr int BlockSize = 3000;
auto solve() {
  // 读入a数组并建立a_id映射
  auto arrSize = read();
  auto qrySize = read();
  auto a = vector(arrSize + 1, 0);
  auto cnt = vector(arrSize + 1, 0);
  auto ans = vector(qrySize + 1, 0LL);
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
  st[0].resize(arrSize + 1);
  for (auto i : iota(1, arrSize + 1)) {
    int x = read();
    st[0][i] = array{x, x};
  }
  // 初始化ST表
  for (int i : iota(1, stSize)) {
    const int curSize = arrSize - (1 << i) + 1;
    st[i].reserve(curSize + 1);
    st[i].emplace_back(array{0, 0});
    for (int j : iota(1, curSize + 1)) {
      st[i].emplace_back(
          minmaxUpd(st[i - 1][j], st[i - 1][j + (1 << (i - 1))]));
    }
  }
  // 通过st表查询y_k的[l,r]区间的minmax信息
  auto stQry = [&](int const l, int const r) {
    int k = bit_width(static_cast<uint32_t>(r - l + 1)) - 1;
    return minmaxUpd(st[k][l], st[k][r - (1 << k) + 1]);
  };
  // ST表 end
  // 莫队 begin
  auto qryArr = iota(1, qrySize + 1) | to<vector>();
  auto lQry = vector(qrySize + 1, 0);
  auto rQry = vector(qrySize + 1, 0);
  for (auto &&[l, r] : zip(lQry, rQry)) {
    l = read(), r = read(), ++cnt[l];
  }
  sort(qryArr,
       [&rQry](int const &i, int const &j) { return rQry[i] < rQry[j]; });
  auto lId = vector(arrSize + 1, vector<int>());
  for (int i : iota(1, arrSize + 1)) {
    lId[i].reserve(cnt[i]), cnt[i] = 0;
  }
  for (auto i = 1; auto const &l : lQry) {
    lId[l].emplace_back(i), ++i;
  }
  // 莫队 end
  // 小于BlockSize的情况
  {
    // 立方根分块前缀和 begin
    auto sum1 = vector(arrSize + 1, 0);
    auto sum2 = vector((arrSize >> 6) + 1, 0);
    auto sum3 = vector((arrSize >> 12) + 1, 0);
    auto sumUpd = [&sum1, &sum2, &sum3] [[gnu::always_inline]] (int const &i) {
      ++sum1[i], ++sum2[i >> 6], ++sum3[i >> 12];
    };
    auto sumQry = [&](int const i) {
      auto res = 0;
      for (auto const k : iota((i >> 6) << 6, i + 1)) {
        res += sum1[k];
      }
      for (auto const k : iota((i >> 12) << 6, i >> 6)) {
        res += sum2[k];
      }
      for (auto const k : iota(0, i >> 12)) {
        res += sum3[k];
      }
      return res;
    };
    // 立方根分块前缀和 end
    auto viewLe = iota(1, arrSize) | filter([&](int const &i) {
                    return aId[i].size() < BlockSize;
                  });
    auto curRange = vector(arrSize + 1, vector<array<int, 2>>());
    auto lastMin = vector(arrSize + 1, vector<int>());
    auto rk = vector(arrSize + 1, 0);
    for (auto const i : viewLe) {
      auto const &cur = aId[i];
      auto const curSize = static_cast<int>(cur.size());
      curRange[i].reserve(curSize);
      for (auto const j : iota(0, curSize)) {
        rk[cur[j]] = j;
        if (j != curSize - 1) {
          auto res = stQry(cur[j], cur[j + 1]);
          curRange[i].emplace_back(res);
          ++cnt[res[0]];
        }
      }
    }
    for (auto const i : iota(1, arrSize + 1)) {
      lastMin[i].reserve(cnt[i]);
    }
    for (auto const i : viewLe) {
      auto const &cur = aId[i];
      auto const curSize = static_cast<int>(cur.size());
      for (auto const j : iota(0, curSize - 1)) {
        lastMin[stQry(cur[j], cur[j + 1])[0]].emplace_back(i);
      }
    }
    auto updLe = [&](int const &i) {
      for (auto const &id : lastMin[i]) {
        auto const val = a[id];
        auto const curRk = rk[id];
        auto const &cur = curRange[val];
        for (auto mx1 = 0; auto l : iota(0, cur[curRk][1] + 1) | reverse) {
          if (l < curRk && cur[l][0] <= i) {
            break;
          }
          mx1 = max(mx1, cur[l][0]);
          for (auto mx2 = mx1; auto &&[mnR, mxR] : cur | drop(curRk)) {
            if (mnR <= i) {
              break;
            }
            mx2 = max(mx2, mxR), sumUpd(mx2);
          }
        }
      }
      for (auto const &id : lId[i]) {
        ans[id] += sumQry(rQry[id]);
      }
    };
    for (auto i : iota(1, arrSize + 1) | reverse) {
      updLe(i);
    }
  }
  // 大于BlockSize的情况
  auto idVal = vector<int>();
  idVal.reserve(arrSize + 1);
  auto sta_mem = vector<int>();
  sta_mem.reserve(arrSize);
  auto sta = stack(std::move(sta_mem));
  auto updGe = [&](int const &i) {
    auto const &cur = aId[i];
    auto const curSize = static_cast<int>(cur.size());
    auto mnmx = vector(curSize - 1, array<int, 2>());
    auto mnmxL = vector(curSize - 1, array<int, 2>());
    auto mnmxR = vector(curSize - 1, array<int, 2>());
    auto lTemp = vector(arrSize + 1, 0);
    auto rTemp = vector(arrSize + 1, 0);
    auto sizVal = 0;
    auto sizQVal = 0;
    { // Init
      auto visVal = vector(arrSize + 1, false);
      auto lVal = vector(arrSize + 1, 0);
      auto rVal = vector(arrSize + 1, 0);
      idVal.push_back(0);
      for (auto const j : iota(0, curSize - 1)) {
        mnmx[j] = stQry(cur[j], cur[j + 1]);
        visVal[mnmx[j][0]] = true, visVal[mnmx[j][1]] = true;
      }
      for (auto const j : iota(1, arrSize + 1)) {
        if (visVal[j]) {
          idVal.push_back(j), ++sizVal;
          lVal[sizVal] = j, rVal[sizVal] = j;
        }
      }
      for (auto const j : iota(1, arrSize + 1)) {
        if (!visVal[j]) {
          lVal[j] = lVal[j - 1];
        }
      }
      for (auto const j : iota(1, arrSize + 1) | reverse) {
        if (!visVal[j]) {
          rVal[j] = rVal[j + 1];
        }
      }
      idVal.clear();
      for (auto j : iota(0, curSize - 1)) {
        auto &l = mnmx[j][0];
        auto &r = mnmx[j][1];
        l = lVal[l], r = rVal[r];
        mnmxL[j][mnmx[j][0] != 0] = l;
        mnmxR[j][mnmx[j][1] != 0] = r;
      }
      for (auto const &j : qryArr) {
        if (lQry[j] <= idVal[i] && idVal[1] <= rQry[j] &&
            rVal[lQry[j]] <= lVal[rQry[j]]) {
          ++sizQVal;
          lTemp[sizQVal] = rVal[lQry[j]], rTemp[sizQVal] = lVal[rQry[j]];
        }
      }
      if (sizQVal == 0) {
        return;
      }
    }
    // 块状链表
    auto lBlock = vector(curSize + 1, 0);
    auto rBlock = vector(curSize + 1, 0);
    const auto curBlock = static_cast<int>(sizVal / sqrt(sizQVal)) + 1;
    auto getcurBlock = [&](int const &i) { return ((i - 1) / curBlock) + 1; };
    auto link = [&](int const &i) {
      int64_t res = 1LL * (i - lBlock[i] + 1) * (rBlock[i + 1] - i + 1);
      rBlock[lBlock[i]] = rBlock[i + 1];
      lBlock[rBlock[i + 1]] = lBlock[i];
      sta.emplace(i);
      return res;
    };
    for (auto const curblock : iota(1, sizQVal + 1) | chunk(curBlock)) {
    }
  };
  for (auto const i : iota(1, arrSize + 1)) {
    if (aId[i].size() >= BlockSize) {
      updGe(i);
    }
  }
  for (auto const &i : ans | drop(1)) {
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