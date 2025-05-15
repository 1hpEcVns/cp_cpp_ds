#include <cstdint>
#include <cstdio>
#include <cstdlib>

constexpr int N_io = 1e5 + 1;

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

// 综合考察了分块 离散化 回滚莫队 块状链表 ST表

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
  auto cnt = vector(max(arrSize, qrySize) + 1, 0);
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
  for (auto &&[l, r] : zip(lQry, rQry) | drop(1)) {
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
        lastMin[stQry(cur[j], cur[j + 1])[0]].emplace_back(cur[j]);
      }
    }
    for (auto i : iota(1, arrSize + 1) | reverse) {
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
            if (mnR < i) {
              break;
            }
            mx2 = max(mx2, mxR), sumUpd(mx2);
          }
        }
      }
      for (auto const &id : lId[i]) {
        ans[id] += sumQry(rQry[id]);
      }
    }
  }
  // 大于BlockSize的情况
  auto idVal = vector<int>();
  auto lUQ = vector<int>();
  auto rUQ = vector<int>();
  idVal.reserve(arrSize + 1);
  lUQ.reserve(arrSize + 1);
  rUQ.reserve(arrSize + 1);
  auto lPtr = vector(arrSize + 1, 0);
  auto rPtr = vector(arrSize + 1, 0);
  auto qId = vector(qrySize + 1, 0);
  auto sta_mem = vector<int>();
  sta_mem.reserve(arrSize);
  auto sta = stack(std::move(sta_mem));
  for (auto const val : iota(1, arrSize + 1)) {
    if (aId[val].size() >= BlockSize) {
      auto const &vId = aId[val]; // 所有val值的位置
      auto const vSize = static_cast<int>(vId.size());
      auto mnmx = vector(vSize - 1, array<int, 2>());
      auto mnmxL = vector(vSize - 1, array<int, 2>());
      auto mnmxR = vector(vSize - 1, array<int, 2>());
      auto sizVal = 0;                            // 离散化的区间限制的个数
      auto sizQVal = 0;                           // 满足条件的询问的个数
      {                                           // Init
        auto visVal = vector(arrSize + 1, false); // 被离散化的值
        auto lVal = vector(arrSize + 1, 0);
        auto rVal = vector(arrSize + 1, 0);
        idVal.push_back(0); // 离散化的区间限制对应的原始值
        for (auto const j : iota(0, vSize - 1)) {
          // 使得点对vId[j]和vId[j+1]产生贡献所需要的限制
          mnmx[j] = stQry(vId[j], vId[j + 1]);
          // 离散化限制的值
          visVal[mnmx[j][0]] = true, visVal[mnmx[j][1]] = true;
        }
        for (auto const i : iota(1, arrSize + 1)) {
          if (visVal[i]) {
            idVal.push_back(i), ++sizVal;
            lVal[sizVal] = i, rVal[sizVal] = i;
          }
        }
        for (auto const i : iota(1, arrSize + 1)) {
          if (!lVal[i]) { // 到可以向左扩展贡献的限制的指针
            lVal[i] = i != 1 ? lVal[i - 1] : 0;
          }
        }
        for (auto const i : iota(1, arrSize + 1) | reverse) {
          if (!rVal[i]) { // 到可以向右扩展贡献的限制的指针
            rVal[i] = i != arrSize ? rVal[i + 1] : 0;
          }
        }
        for (auto const i : iota(0, vSize - 1)) {
          auto &l = mnmx[i][0];
          auto &r = mnmx[i][1];
          l = lVal[l], r = rVal[r];
          mnmxL[i][mnmx[i][0] != 0] = l;
          mnmxR[i][mnmx[i][1] != 0] = r;
        }
        for (auto const &i : qryArr) {
          if (lQry[i] <= idVal[sizVal] &&
              idVal[1] <= rQry[i] &&            // 在值为val的点限制的区间内
              rVal[lQry[i]] <= lVal[rQry[i]]) { // 可以相交
            ++sizQVal;
            lUQ.emplace_back(rVal[lQry[i]]);
            rUQ.emplace_back(lVal[rQry[i]]);
            qId[sizQVal] = i;
          }
        }
        idVal.clear();
        if (sizQVal == 0) {
          return;
        }
      }
      // 块状链表
      const auto curBlock = static_cast<int>(sizVal / sqrt(sizQVal)) + 1;
      for (auto const block : iota(1, sizQVal + 1) | chunk(curBlock)) {
        auto const blockR = min(block.front() + sizQVal - 1, sizQVal);
        auto curR = blockR + 1;
        for (auto const j : iota(1, vSize)) {
          lPtr[j] = j, rPtr[j] = j;
        } // 初始化链表
        auto sum = 0LL;
        auto link = [&] [[gnu::always_inline]] (int const i) {
          sum += 1LL * (i - lPtr[i] + 1) * (rPtr[i + 1] - i + 1);
          rPtr[lPtr[i]] = rPtr[i + 1];
          lPtr[rPtr[i + 1]] = lPtr[i];
        };
        auto cut = [&]() {
          while (!sta.empty()) {
            auto const i = sta.top();
            rPtr[lPtr[i]] = i;
            lPtr[rPtr[i + 1]] = i + 1;
            sta.pop();
          }
        };
        for (auto const id : block) {
          auto &curAns = ans[qId[id]];
          const auto qUR = rUQ[id]; // 离散化后的询问右端点
          const auto qUL = lUQ[id]; // 离散化后的询问左端点
          if (qUR < blockR) {       // 在区间内
            for (auto const i : iota(qUL, qUR + 1)) {
              for (auto const &j : mnmxL[i]) {
                if (j && mnmx[j][1] <= qUR) {
                  link(j), sta.emplace(j);
                }
              }
            }
            curAns += sum, sum = 0, cut();
          } else { // 在区间外
            while (curR <= qUR) {
              for (auto const &i : mnmxR[curR]) {
                if (blockR < mnmx[i][0]) {
                  link(i);
                }
              }
              ++curR;
            }
            auto last = sum;
            for (auto const i : iota(qUL, blockR + 1) | reverse) {
              for (auto const &j : mnmxL[i]) {
                if (qUL <= mnmx[j][0]) {
                  link(j);
                }
              }
            }
            curAns += sum, sum = last, cut();
          }
        }
        lUQ.clear();
        rUQ.clear();
      }
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