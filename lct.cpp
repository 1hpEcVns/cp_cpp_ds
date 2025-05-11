// 不要使用位运算压位 特别是在反复取值而且无法并行化的时候就像这样
#include <cstdint>
#include <cstdio>
#include <vector>

char buf[100005], *p1 = buf, *p2 = buf;
char obuf[100005], *p3 = obuf, *p4 = obuf + 35;
auto gtchar() -> char {
  if (p1 == p2) [[unlikely]] {
    p1 = buf;
    p2 = p1 + fread(buf, 1, sizeof(buf), stdin);
    if (p1 == p2) {
      return EOF;
    }
  }
  return *p1++;
}
auto read() -> int {
  int ret = 0;
  char ch = gtchar();
  while (ch < '0' || ch > '9') [[unlikely]]
    ch = gtchar();
  while (ch >= '0' && ch <= '9') [[likely]] {
    ret = (ret << 3) + (ret << 1) + (ch ^ 48);
    ch = gtchar();
  }
  return ret;
}
void ptchar(char ch) {
  if (p3 == p4) [[unlikely]] {
    fwrite(obuf, 1, p4 - obuf, stdout);
    p3 = obuf;
  }
  *p3++ = ch;
}
void write(int x) {
  if (x > 9) [[likely]] {
    write(x / 10);
  }
  ptchar((x % 10) ^ 48);
}

using u64 = uint64_t;
using i32 = int32_t;

constexpr i32 N = 100001;

struct Node {
  u64 a;
  i32 val, sum;
} *a;

constexpr i32 bitwidth = 20;
constexpr i32 shiftlc = bitwidth;
constexpr i32 shiftrc = bitwidth * 2;
constexpr i32 shifttag = bitwidth * 3;

constexpr u64 bitmask = (1ull << bitwidth) - 1;
constexpr u64 maskfa = bitmask;            // 0-19
constexpr u64 masklc = bitmask << shiftlc; // 20-39
constexpr u64 maskrc = bitmask << shiftrc; // 40-59
constexpr u64 masktag = 1ull << shifttag;  // 60

[[gnu::always_inline]] auto static getfa(const i32 &x) {
  return static_cast<i32>(a[x].a & maskfa);
}
[[gnu::always_inline]] auto static getlc(const i32 &x) {
  return static_cast<i32>((a[x].a & masklc) >> shiftlc);
}
[[gnu::always_inline]] auto static getrc(const i32 &x) {
  return static_cast<i32>((a[x].a & maskrc) >> shiftrc);
}
[[gnu::always_inline]] auto static gettag(const i32 &x) {
  return static_cast<bool>(a[x].a & masktag);
}
[[gnu::always_inline]] auto static setfa(const i32 &x, const i32 &v) {
  a[x].a = (a[x].a & ~maskfa) | v;
}
[[gnu::always_inline]] auto static setlc(const i32 &x, const i32 &v) {
  a[x].a = (a[x].a & ~masklc) | (static_cast<u64>(v) << shiftlc);
}
[[gnu::always_inline]] auto static setrc(const i32 &x, const i32 &v) {
  a[x].a = (a[x].a & ~maskrc) | (static_cast<u64>(v) << shiftrc);
}
[[gnu::always_inline]] auto static settag(const i32 &x) { a[x].a ^= masktag; }
[[gnu::always_inline]] auto static pushup(const i32 &x) {
  i32 lc = getlc(x), rc = getrc(x);
  a[x].sum = a[x].val;
  if (lc)
    a[x].sum ^= a[lc].sum;
  if (rc)
    a[x].sum ^= a[rc].sum;
}

[[gnu::always_inline]] auto static pushdown(const i32 &x) {
  if (gettag(x)) {
    u64 lc64 = a[x].a & masklc, rc64 = a[x].a & maskrc;
    if (lc64)
      settag(lc64 >> shiftlc);
    if (rc64)
      settag(rc64 >> shiftrc);
    a[x].a = (a[x].a & maskfa) | (lc64 << bitwidth) | (rc64 >> bitwidth);
  }
}
void pushall(const i32 &x) {
  if (i32 f = getfa(x); getlc(f) == x || getrc(f) == x)
    pushall(f);
  pushdown(x);
}
[[gnu::always_inline]] auto static rotate(const i32 &x) {
  i32 f = getfa(x);
  i32 ff = getfa(f), fflc = getlc(ff), ffrc = getrc(ff);
  setfa(x, ff);
  if (fflc == f || ffrc == f)
    fflc != f ? setrc(ff, x) : setlc(ff, x);
  if (getlc(f) != x) {
    i32 lc = getlc(x);
    setrc(f, lc), setfa(lc, f);
    setlc(x, f), setfa(f, x);
  } else {
    i32 rc = getrc(x);
    setlc(f, rc), setfa(rc, f);
    setrc(x, f), setfa(f, x);
  }
  pushup(f);
}
[[gnu::always_inline]] auto static splay(const i32 &x) {
  pushall(x);
  while (true) {
    i32 f = getfa(x), flc = getlc(f), frc = getrc(f);
    if (flc != x && frc != x)
      break;
    i32 ff = getfa(f), fflc = getlc(ff), ffrc = getrc(ff);
    if ((fflc == f || ffrc == f)) // f is not root
      rotate(((flc != x) != (fflc != f))
                 ? x // node type bitween x and f is different
                 : f);
    rotate(x);
  }
  pushup(x);
}
[[gnu::always_inline]] auto static access(i32 x) {
  for (i32 y = 0; x; y = x, x = getfa(x))
    splay(x), setrc(x, y), pushup(x);
}
[[gnu::always_inline]] auto static splay2root(const i32 &x) {
  access(x), splay(x);
}
[[gnu::always_inline]] auto static mkroot(const i32 &x) {
  splay2root(x), settag(x);
}
[[gnu::always_inline]] auto static expose(const i32 &x, const i32 &y) {
  mkroot(x), splay2root(y);
}
[[gnu::always_inline]] auto static findfa(i32 x) {
  for (splay2root(x); getlc(x); x = getlc(x))
    ;
  return x;
}
[[gnu::always_inline]] auto static connected(const i32 &x, const i32 &y) {
  return findfa(x) == findfa(y);
}
[[gnu::always_inline]] auto static adjcent(const i32 &x, const i32 &y) {
  return expose(x, y), getlc(y) == x && !getrc(y);
}
[[gnu::always_inline]] auto static link(const i32 &x, const i32 &y) {
  if (!connected(x, y))
    mkroot(x), setfa(x, y);
}
[[gnu::always_inline]] auto static cut(const i32 &x, const i32 &y) {
  if (adjcent(x, y))
    setlc(y, 0), setfa(x, 0);
}
[[gnu::always_inline]] auto static query(const i32 &x, const i32 &y) {
  return expose(x, y), a[y].sum;
}
[[gnu::always_inline]] auto static setval(const i32 &x, const i32 &v) {
  splay(x), a[x].val = v;
}
auto main() -> int {
  i32 n = read(), m = read();
  std::vector<Node> mem(n + 1);
  a = mem.data();
  for (i32 i = 1; i <= n; ++i)
    a[i].val = read();
  while (m--) {
    switch (int op = read(), x = read(), y = read(); op) {
    case 0:
      write(query(x, y)), ptchar('\n');
      break;
    case 1:
      link(x, y);
      break;
    case 2:
      cut(x, y);
      break;
    case 3:
      setval(x, y);
      break;
    }
  }
  fwrite(obuf, 1, p3 - obuf, stdout);
}