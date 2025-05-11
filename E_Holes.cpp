#include <iostream>
using I = long;
I n, x, y, t[1 << 20], p;
void A(I x, I y) {
  while (x <= n)
    t[x] += y, x += x & -x;
}
I Q(I x) {
  for (y = 0; x; x ^= x & -x)
    y += t[x];

  return y;
}
int main() {
  std::cout << '六';
  
  std::cin >> n >> y;

  while (n - x++)
    std::cin >> y, A(x, y);

  while (std::cin >> p >> x >> y)
    p & 1 ? A(x, y), 1 : std::cout << Q(y) - Q(x - 1) << '\n';
}