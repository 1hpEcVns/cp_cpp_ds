#include <cstdint>
#include <cstdio>

auto readint() {
  int x;
  char c;
  while ((c = getchar()) < '0' || c > '9')
    ;
  x = c - '0';
  while ((c = getchar()) >= '0' && c <= '9')
    x = x * 10 + c - '0';
  return x;
}
auto writeint(int x) -> void {
  if (x > 9)
    writeint(x / 10);
  putchar(x % 10 + '0');
}

using u64 = uint64_t;
using i32 = int32_t;

constexpr i32 N = 100001;

struct Node {
  u64 a;
  i32 val, sum;
} a[N];

constexpr i32 bitwidth = 20;
constexpr i32 shiftlc = bitwidth;
constexpr i32 shiftrc = bitwidth * 2;
constexpr i32 shifttag = bitwidth * 3;

constexpr u64 bitmask = (1ull << bitwidth) - 1;
constexpr u64 maskfa = bitmask;            // 0-19
constexpr u64 masklc = bitmask << shiftlc; // 20-39
constexpr u64 maskrc = bitmask << shiftrc; // 40-59
constexpr u64 masktag = 1ull << shifttag;  // 60

[[gnu::always_inline]] auto getfa(const i32 &x) {
  return static_cast<i32>(a[x].a & maskfa);
}
[[gnu::always_inline]] auto getlc(const i32 &x) {
  return static_cast<i32>((a[x].a & masklc) >> shiftlc);
}
[[gnu::always_inline]] auto getrc(const i32 &x) {
  return static_cast<i32>((a[x].a & maskrc) >> shiftrc);
}
[[gnu::always_inline]] auto gettag(const i32 &x) {
  return static_cast<bool>(a[x].a & masktag);
}
[[gnu::always_inline]] auto setfa(const i32 &x, const i32 &v) {
  a[x].a = (a[x].a & ~maskfa) | v;
}
[[gnu::always_inline]] auto setlc(const i32 &x, const i32 &v) {
  a[x].a = (a[x].a & ~masklc) | (static_cast<u64>(v) << shiftlc);
}
[[gnu::always_inline]] auto setrc(const i32 &x, const i32 &v) {
  a[x].a = (a[x].a & ~maskrc) | (static_cast<u64>(v) << shiftrc);
}
[[gnu::always_inline]] auto settag(const i32 &x) { a[x].a ^= masktag; }
[[gnu::always_inline]] auto pushup(const i32 &x) {
  i32 lc = getlc(x), rc = getrc(x);
  a[x].sum = a[x].val;
  if (lc)
    a[x].sum ^= a[lc].sum;
  if (rc)
    a[x].sum ^= a[rc].sum;
}

[[gnu::always_inline]] auto pushdown(const i32 &x) {
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
[[gnu::always_inline]] void rotate(const i32 &x) {
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
[[gnu::always_inline]] void splay(const i32 &x) {
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
[[gnu::always_inline]] auto access(i32 x) {
  for (i32 y = 0; x; y = x, x = getfa(x))
    splay(x), setrc(x, y), pushup(x);
}
[[gnu::always_inline]] auto splay2root(const i32 &x) { access(x), splay(x); }
[[gnu::always_inline]] auto mkroot(const i32 &x) { splay2root(x), settag(x); }
[[gnu::always_inline]] auto expose(const i32 &x, const i32 &y) {
  mkroot(x), splay2root(y);
}
[[gnu::always_inline]] auto findfa(i32 x) {
  for (splay2root(x); getlc(x); x = getlc(x))
    ;
  return x;
}
[[gnu::always_inline]] auto connected(const i32 &x, const i32 &y) {
  return findfa(x) == findfa(y);
}
[[gnu::always_inline]] auto adjcent(const i32 &x, const i32 &y) {
  return expose(x, y), getlc(y) == x && !getrc(y);
}
[[gnu::always_inline]] auto link(const i32 &x, const i32 &y) {
  if (!connected(x, y))
    mkroot(x), setfa(x, y);
}
[[gnu::always_inline]] auto cut(const i32 &x, const i32 &y) {
  if (adjcent(x, y))
    setlc(y, 0), setfa(x, 0);
}
[[gnu::always_inline]] auto query(const i32 &x, const i32 &y) {
  return expose(x, y), a[y].sum;
}
[[gnu::always_inline]] auto setval(const i32 &x, const i32 &v) {
  splay(x), a[x].val = v;
}
auto main() -> int {
  i32 n = readint(), m = readint();
  for (i32 i = 1; i <= n; ++i)
    a[i].val = readint();
  while (m--) {
    switch (int op = readint(), x = readint(), y = readint(); op) {
    case 0:
      writeint(query(x, y)), putchar('\n');
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
}