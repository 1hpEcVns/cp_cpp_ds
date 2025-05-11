#include <cstdio>

const int N_io = 1e6 + 10;

char *p1, *p2, buf[N_io];                                            // NOLINT
[[nodiscard]] inline char nc() noexcept {                         // NOLINT
  return (p1 == p2 && (p2 = (p1 = buf) + fread(buf, 1, N_io, stdin), // NOLINT
                       p1 == p2)                                  // NOLINT
              ? EOF
              : *p1++); // NOLINT
}
int read() {                   // NOLINT
  int x = 0, f = 1;            // NOLINT
  char ch = nc();              // NOLINT
  while (ch < 48 || ch > 57) { // NOLINT
    if (ch == '-')             // NOLINT
      f = -1;                  // NOLINT
    ch = nc();                 // NOLINT
  } // NOLINT
  while (ch >= 48 && ch <= 57)                      // NOLINT
    x = (x << 3) + (x << 1) + (ch ^ 48), ch = nc(); // NOLINT
  return x * f;                                     // NOLINT
}

char obuf[N_io], *p3 = obuf; // NOLINT
inline void putchar(char x) { // NOLINT
  (p3 - obuf < N_io)         // NOLINT
      ? (*p3++ = x)          // NOLINT
      : (fwrite(obuf, p3 - obuf, 1, stdout), p3 = obuf, *p3++ = x); // NOLINT
}
inline void write(int x) { // NOLINT
  if (!x) {                // NOLINT
    putchar('0');          // NOLINT
    return;                // NOLINT
  } // NOLINT
  int len = 0, k1 = x, c[40];          // NOLINT
  if (k1 < 0)                          // NOLINT
    k1 = -k1, putchar('-');            // NOLINT
  while (k1)                           // NOLINT
    c[len++] = k1 % 10 ^ 48, k1 /= 10; // NOLINT
  while (len--)                        // NOLINT
    putchar(c[len]);                   // NOLINT
}

//以上为快读快写

#include <vector>
#include <array>

using namespace std;

auto main() -> int {
  auto n = read(), m = read();

  auto a=vector(n,array{0,0,0})

  while (m--) {
    
  }
}
