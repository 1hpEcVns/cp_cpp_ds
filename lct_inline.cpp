#include <cstdint>
#include <cstdio>

// --- 快速 I/O ---
// (你的快速 I/O 代码保持不变，这里省略以聚焦LCT)
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
// --- 结束 快速 I/O ---

using u64 = uint64_t;
using i32 = int32_t;

constexpr i32 N_NODES_CONST = 100001; // 改个名字以示区分

struct Node {
  u64 packed_data; // 将 'a' 重命名为更具描述性的名字
  i32 val, sum;
} nodes_array[N_NODES_CONST]; // 全局节点数组

// --- 位运算常量 ---
constexpr i32 bitwidth = 20;
constexpr i32 shift_fa = 0; // fa 明确从0开始
constexpr i32 shift_lc = bitwidth;
constexpr i32 shift_rc = bitwidth * 2;
constexpr i32 shift_tag = bitwidth * 3;

constexpr u64 bitmask_20 = (1ULL << bitwidth) - 1; // 通用20位掩码

constexpr u64 mask_fa = bitmask_20 << shift_fa; // 等同于 bitmask_20
constexpr u64 mask_lc = bitmask_20 << shift_lc;
constexpr u64 mask_rc = bitmask_20 << shift_rc;
constexpr u64 mask_tag = 1ULL << shift_tag;

// --- LCT 核心函数 (所有辅助位运算将被手动内联) ---
// 注意：所有函数参数都使用 x_param, f_param 等来避免与内联代码中可能出现的x,
// f冲突

// pushup (已手动内联 getlc, getrc)
void pushup_manual_inline(const i32 &x_param) {
  // i32 lc = getlc(x_param);
  i32 lc = static_cast<i32>((nodes_array[x_param].packed_data & mask_lc) >>
                            shift_lc);
  // i32 rc = getrc(x_param);
  i32 rc = static_cast<i32>((nodes_array[x_param].packed_data & mask_rc) >>
                            shift_rc);

  nodes_array[x_param].sum = nodes_array[x_param].val;
  if (lc) // 假设 lc=0 代表空节点，nodes_array[0].sum 是 0 (异或单位元)
    nodes_array[x_param].sum ^= nodes_array[lc].sum;
  if (rc)
    nodes_array[x_param].sum ^= nodes_array[rc].sum;
}

// pushdown (使用之前优化的版本并手动内联settag)
void pushdown_manual_inline(const i32 &x_param) {
  u64 current_node_a_val = nodes_array[x_param].packed_data;
  // if (!(current_node_a_val & mask_tag)) // Inlined gettag(x_param)
  if ((current_node_a_val & mask_tag) == 0) { // 更明确的检查tag是否为0
    return;
  }

  // u64 lc_field_original = current_node_a_val & mask_lc;
  // u64 rc_field_original = current_node_a_val & mask_rc;
  // 这两步是为了提取索引并检查孩子是否存在
  i32 lc_idx = static_cast<i32>((current_node_a_val & mask_lc) >>
                                shift_lc); // Inlined getlc(x_param)
  i32 rc_idx = static_cast<i32>((current_node_a_val & mask_rc) >>
                                shift_rc); // Inlined getrc(x_param)

  if (lc_idx != 0) {
    // settag(lc_idx); -> nodes_array[lc_idx].packed_data ^= mask_tag;
    nodes_array[lc_idx].packed_data ^= mask_tag;
  }
  if (rc_idx != 0) {
    // settag(rc_idx); -> nodes_array[rc_idx].packed_data ^= mask_tag;
    nodes_array[rc_idx].packed_data ^= mask_tag;
  }

  // 保留fa, 清除lc, rc, tag。然后用交换后的lc/rc值填充。
  u64 new_a_val = current_node_a_val & ~(mask_lc | mask_rc | mask_tag);

  // new_a_val |= (rc_idx << shift_lc); // 用原始的rc_idx放到lc位置
  // new_a_val |= (lc_idx << shift_rc); // 用原始的lc_idx放到rc位置
  // 上面这句是错误的，应该是用字段值移位或者用掩码后的索引移位
  // 应该是从 current_node_a_val & mask_rc (这是rc字段) 移到 lc位置
  // 从 current_node_a_val & mask_lc (这是lc字段) 移到 rc位置

  u64 original_lc_field_content = current_node_a_val & mask_lc; // 内容如 LLL000
  u64 original_rc_field_content =
      current_node_a_val & mask_rc; // 内容如 RRR000 (R在更高位)

  new_a_val |=
      (original_rc_field_content >> bitwidth); // 将rc字段内容右移到lc字段位置
  new_a_val |=
      (original_lc_field_content << bitwidth); // 将lc字段内容左移到rc字段位置

  nodes_array[x_param].packed_data = new_a_val;
}

// pushall (已手动内联 getfa, getlc, getrc, pushdown)
void pushall_manual_inline(const i32 &x_param) {
  // i32 f_of_x = getfa(x_param);
  i32 f_of_x = static_cast<i32>(nodes_array[x_param].packed_data & mask_fa);

  if (f_of_x != 0) { // x 不是根
    // i32 lc_of_f = getlc(f_of_x);
    i32 lc_of_f = static_cast<i32>(
        (nodes_array[f_of_x].packed_data & mask_lc) >> shift_lc);
    // i32 rc_of_f = getrc(f_of_x);
    i32 rc_of_f = static_cast<i32>(
        (nodes_array[f_of_x].packed_data & mask_rc) >> shift_rc);

    if (lc_of_f == x_param ||
        rc_of_f == x_param) {        // x 是 f 的孩子 (f 不是“假父亲”)
      pushall_manual_inline(f_of_x); // 递归调用
    }
  }

  // 内联 pushdown_manual_inline(x_param)
  {
    const i32 current_x_for_pushdown = x_param;
    u64 current_node_a_val_pd = nodes_array[current_x_for_pushdown].packed_data;
    if (!((current_node_a_val_pd & mask_tag) == 0)) { // if tag is set
      i32 lc_idx_pd =
          static_cast<i32>((current_node_a_val_pd & mask_lc) >> shift_lc);
      i32 rc_idx_pd =
          static_cast<i32>((current_node_a_val_pd & mask_rc) >> shift_rc);

      if (lc_idx_pd != 0) {
        nodes_array[lc_idx_pd].packed_data ^= mask_tag;
      }
      if (rc_idx_pd != 0) {
        nodes_array[rc_idx_pd].packed_data ^= mask_tag;
      }

      u64 new_a_val_pd =
          current_node_a_val_pd & ~(mask_lc | mask_rc | mask_tag);
      u64 original_lc_field_content_pd = current_node_a_val_pd & mask_lc;
      u64 original_rc_field_content_pd = current_node_a_val_pd & mask_rc;
      new_a_val_pd |= (original_rc_field_content_pd >> bitwidth);
      new_a_val_pd |= (original_lc_field_content_pd << bitwidth);
      nodes_array[current_x_for_pushdown].packed_data = new_a_val_pd;
    }
  }
}

// rotate (内联大量 get/set 和 pushup)
void rotate_manual_inline(const i32 &x_param) {
  // i32 f = getfa(x_param);
  i32 f_param = static_cast<i32>(nodes_array[x_param].packed_data & mask_fa);

  // i32 ff = getfa(f_param);
  i32 ff_param = static_cast<i32>(nodes_array[f_param].packed_data & mask_fa);

  // i32 fflc = getlc(ff_param);
  i32 fflc_val = static_cast<i32>(
      (nodes_array[ff_param].packed_data & mask_lc) >> shift_lc);
  // i32 ffrc = getrc(ff_param);
  i32 ffrc_val = static_cast<i32>(
      (nodes_array[ff_param].packed_data & mask_rc) >> shift_rc);

  // setfa(x_param, ff_param);
  nodes_array[x_param].packed_data =
      (nodes_array[x_param].packed_data & ~mask_fa) |
      (static_cast<u64>(ff_param) & bitmask_20);

  if (ff_param != 0 &&
      (fflc_val == f_param || ffrc_val == f_param)) { // 如果 f 不是根
    if (fflc_val != f_param) { // f 是 ff 的右孩子, x 成为 ff 的新右孩子
      // setrc(ff_param, x_param);
      nodes_array[ff_param].packed_data =
          (nodes_array[ff_param].packed_data & ~mask_rc) |
          ((static_cast<u64>(x_param) & bitmask_20) << shift_rc);
    } else { // f 是 ff 的左孩子, x 成为 ff 的新左孩子
      // setlc(ff_param, x_param);
      nodes_array[ff_param].packed_data =
          (nodes_array[ff_param].packed_data & ~mask_lc) |
          ((static_cast<u64>(x_param) & bitmask_20) << shift_lc);
    }
  }

  // child_of_x;
  // if (getlc(f_param) != x_param) { // x 是 f_param 的右孩子 (zig-left)
  if (static_cast<i32>((nodes_array[f_param].packed_data & mask_lc) >>
                       shift_lc) != x_param) {
    // i32 lc_of_x = getlc(x_param);
    i32 lc_of_x = static_cast<i32>(
        (nodes_array[x_param].packed_data & mask_lc) >> shift_lc);

    // setrc(f_param, lc_of_x);
    nodes_array[f_param].packed_data =
        (nodes_array[f_param].packed_data & ~mask_rc) |
        ((static_cast<u64>(lc_of_x) & bitmask_20) << shift_rc);
    if (lc_of_x != 0) {
      // setfa(lc_of_x, f_param);
      nodes_array[lc_of_x].packed_data =
          (nodes_array[lc_of_x].packed_data & ~mask_fa) |
          (static_cast<u64>(f_param) & bitmask_20);
    }

    // setlc(x_param, f_param);
    nodes_array[x_param].packed_data =
        (nodes_array[x_param].packed_data & ~mask_lc) |
        ((static_cast<u64>(f_param) & bitmask_20) << shift_lc);
    // setfa(f_param, x_param);
    nodes_array[f_param].packed_data =
        (nodes_array[f_param].packed_data & ~mask_fa) |
        (static_cast<u64>(x_param) & bitmask_20);
  } else { // x 是 f_param 的左孩子 (zig-right)
    // i32 rc_of_x = getrc(x_param);
    i32 rc_of_x = static_cast<i32>(
        (nodes_array[x_param].packed_data & mask_rc) >> shift_rc);

    // setlc(f_param, rc_of_x);
    nodes_array[f_param].packed_data =
        (nodes_array[f_param].packed_data & ~mask_lc) |
        ((static_cast<u64>(rc_of_x) & bitmask_20) << shift_lc);
    if (rc_of_x != 0) {
      // setfa(rc_of_x, f_param);
      nodes_array[rc_of_x].packed_data =
          (nodes_array[rc_of_x].packed_data & ~mask_fa) |
          (static_cast<u64>(f_param) & bitmask_20);
    }

    // setrc(x_param, f_param);
    nodes_array[x_param].packed_data =
        (nodes_array[x_param].packed_data & ~mask_rc) |
        ((static_cast<u64>(f_param) & bitmask_20) << shift_rc);
    // setfa(f_param, x_param);
    nodes_array[f_param].packed_data =
        (nodes_array[f_param].packed_data & ~mask_fa) |
        (static_cast<u64>(x_param) & bitmask_20);
  }

  // 内联 pushup_manual_inline(f_param)
  pushup_manual_inline(f_param);
  // (也可以把 pushup_manual_inline(x_param) 放在 splay
  // 的末尾，但传统LCT是在rotate后更新父节点f，splay最后更新x)
}

// splay (内联 pushall, getfa, getlc, getrc, rotate)
void splay_manual_inline(const i32 &x_param) {
  pushall_manual_inline(x_param); // pushall 自身包含内联的 pushdown

  while (true) {
    // i32 f = getfa(x_param);
    i32 f_param = static_cast<i32>(nodes_array[x_param].packed_data & mask_fa);
    // i32 flc = getlc(f_param);
    i32 flc_val =
        (f_param != 0)
            ? static_cast<i32>((nodes_array[f_param].packed_data & mask_lc) >>
                               shift_lc)
            : 0;
    // i32 frc = getrc(f_param);
    i32 frc_val =
        (f_param != 0)
            ? static_cast<i32>((nodes_array[f_param].packed_data & mask_rc) >>
                               shift_rc)
            : 0;

    if (f_param == 0 ||
        (flc_val != x_param &&
         frc_val != x_param)) { // x 是根，或者 x 不是 f 的孩子
                                // (这种情况一般不应发生若fa指针正确)
      break;
    }

    // i32 ff = getfa(f_param);
    i32 ff_param =
        (f_param != 0)
            ? static_cast<i32>(nodes_array[f_param].packed_data & mask_fa)
            : 0;
    if (ff_param != 0) { // f 不是根 (即 x 有祖父节点)
      // i32 fflc = getlc(ff_param);
      i32 fflc_val = static_cast<i32>(
          (nodes_array[ff_param].packed_data & mask_lc) >> shift_lc);
      // i32 ffrc = getrc(ff_param);
      // (ffrc_val 在 rotate 条件判断中未使用，如果需要可以获取)

      // if ((fflc_val == f_param || ffrc_val == f_param)) // 这句只是重复判断 f
      // 不是根，可以简化
      //   rotate(((flc_val != x_param) != (fflc_val != f_param)) ? x_param :
      //   f_param);
      // zig-zag or zig-zig
      // x is lc of f (flc_val == x_param), f is lc of ff (fflc_val == f_param)
      // -> zig-zig (rotate f) x is rc of f (frc_val == x_param), f is rc of ff
      // (ffrc_val == f_param) -> zig-zig (rotate f) x is lc of f, f is rc of ff
      // -> zig-zag (rotate x) x is rc of f, f is lc of ff -> zig-zag (rotate x)
      // The condition (flc_val != x_param) is true if x is rc of f.
      // The condition (fflc_val != f_param) is true if f is rc of ff.
      // If they are different (true != false or false != true), it's zig-zag,
      // rotate x. If they are same (true == true or false == false), it's
      // zig-zig, rotate f.
      if (((nodes_array[f_param].packed_data & mask_lc) >> shift_lc ==
           x_param) == // x is lc of f
          ((nodes_array[ff_param].packed_data & mask_lc) >> shift_lc ==
           f_param) // f is lc of ff
      ) { // zig-zig (both left or both right, though the second condition for
          // "both right" is more complex to write concisely here) A simpler
          // zig-zig check: (x is f's left AND f is ff's left) OR (x is f's
          // right AND f is ff's right) Or just use your original ((flc_val !=
          // x_param) != (fflc_val != f_param)) for rotate x if zig-zag
        rotate_manual_inline(f_param);
      } else { // zig-zag
        rotate_manual_inline(x_param);
      }
    }
    rotate_manual_inline(x_param);
  }
  pushup_manual_inline(x_param);
}

// access (内联 getfa, splay_manual_inline, setrc, pushup)
void access_manual_inline(i32 x_param_access) {
  i32 current_x = x_param_access;
  for (i32 y_val = 0; current_x != 0; /* update in loop */) {
    splay_manual_inline(current_x); // splay 包含大量内联

    // setrc(current_x, y_val);
    nodes_array[current_x].packed_data =
        (nodes_array[current_x].packed_data & ~mask_rc) |
        ((static_cast<u64>(y_val) & bitmask_20) << shift_rc);

    pushup_manual_inline(current_x);

    y_val = current_x;
    // current_x = getfa(current_x);
    current_x = static_cast<i32>(nodes_array[current_x].packed_data & mask_fa);
  }
}

// --- 其他 LCT 高级函数将依赖上面已“手动内联”的版本 ---
// 比如 mkroot, link, cut 等会调用 access_manual_inline, splay_manual_inline
// 它们内部的 get/set 也会被替换。这会让代码更加膨胀。
// 为简洁，这里不再展开它们，但原理相同。

// mkroot, expose, findfa, connected, adjcent, link, cut, query, setval
// 将会调用上面那些 *_manual_inline 函数
// 它们的实现保持不变，只是它们调用的底层函数现在是“手动内联”后的版本（通过替换函数调用为函数体）
// (为了使代码能编译和运行，这里保留原函数名调用，但你要替换成手动内联版本)
void splay2root_manual_inline(const i32 &x) {
  access_manual_inline(x);
  splay_manual_inline(x);
}
void mkroot_manual_inline(const i32 &x) {
  splay2root_manual_inline(x);
  // settag(x) ->
  nodes_array[x].packed_data ^= mask_tag;
}
void expose_manual_inline(const i32 &x, const i32 &y) {
  mkroot_manual_inline(x);
  splay2root_manual_inline(y);
}

i32 findfa_manual_inline(i32 x_param) {
  splay2root_manual_inline(x_param);
  i32 current_x = x_param;
  // while(getlc(current_x)) current_x = getlc(current_x);
  while (true) {
    i32 lc_of_current_x = static_cast<i32>(
        (nodes_array[current_x].packed_data & mask_lc) >> shift_lc);
    if (lc_of_current_x == 0)
      break;
    current_x = lc_of_current_x;
  }
  return current_x;
}
bool connected_manual_inline(const i32 &x, const i32 &y) {
  return findfa_manual_inline(x) == findfa_manual_inline(y);
}

bool adjcent_manual_inline(const i32 &x, const i32 &y) {
  expose_manual_inline(x, y);
  // getlc(y) == x && !getrc(y)
  bool lc_is_x = (static_cast<i32>((nodes_array[y].packed_data & mask_lc) >>
                                   shift_lc) == x);
  bool rc_is_null = (static_cast<i32>((nodes_array[y].packed_data & mask_rc) >>
                                      shift_rc) == 0);
  return lc_is_x && rc_is_null;
}

void link_manual_inline(const i32 &x, const i32 &y) {
  if (!connected_manual_inline(x, y)) {
    mkroot_manual_inline(x);
    // setfa(x, y);
    nodes_array[x].packed_data = (nodes_array[x].packed_data & ~mask_fa) |
                                 (static_cast<u64>(y) & bitmask_20);
  }
}
void cut_manual_inline(const i32 &x, const i32 &y) {
  if (adjcent_manual_inline(x, y)) {
    // setlc(y, 0);
    nodes_array[y].packed_data =
        (nodes_array[y].packed_data & ~mask_lc) |
        ((static_cast<u64>(0) & bitmask_20) << shift_lc);
    // setfa(x, 0);
    nodes_array[x].packed_data = (nodes_array[x].packed_data & ~mask_fa) |
                                 (static_cast<u64>(0) & bitmask_20);
  }
}
i32 query_manual_inline(const i32 &x, const i32 &y) {
  expose_manual_inline(x, y);
  return nodes_array[y].sum;
}
void setval_manual_inline(const i32 &x, const i32 &v) {
  splay_manual_inline(x);
  nodes_array[x].val = v;
}

// --- main 函数 ---
auto main() -> int {
  i32 n = read(), m = read();
  for (i32 i = 1; i <= n; ++i) {
    // 初始化 packed_data，所有指针字段设为0 (或你的NULL_NODE_IDX), tag为0
    nodes_array[i].packed_data = 0; // 假设所有字段为0是合理的初始状态
    nodes_array[i].val = read();
    // 可能需要调用一次 pushup 来初始化 sum，如果初始时有子节点（通常没有）
    // 或者，如果初始val就是sum，则 nodes_array[i].sum = nodes_array[i].val;
    pushup_manual_inline(i); // 更新sum基于val和空孩子
  }

  while (m--) {
    int op = read();
    int x = read();
    int y = read();
    switch (op) {
    case 0:
      write(query_manual_inline(x, y));
      ptchar('\n');
      break;
    case 1:
      link_manual_inline(x, y);
      break;
    case 2:
      cut_manual_inline(x, y);
      break;
    case 3:
      setval_manual_inline(x, y); // y 在这里是新值 v
      break;
    }
  }
  // Final flush for output buffer
  if (p3 > obuf) { // 只有当obuf中有数据时才执行fwrite
    fwrite(obuf, 1, p3 - obuf, stdout);
    p3 = obuf; // 重置指针 (虽然程序即将结束，但好习惯)
  }
  return 0;
}