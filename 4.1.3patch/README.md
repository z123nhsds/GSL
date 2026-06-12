# 1. GSL_SUPPRESS 宏在 MSVC 与 GCC 下的不同展开形式

## 修复前后对比

| 场景 | 宏定义 | 展开示例（传入 `f.6`） |
| --- | --- | --- |
| 修复前（GCC/Clang/新 MSVC） | `#define GSL_SUPPRESS(x) [[gsl::suppress (x)]]` | `[[gsl::suppress (f.6)]]` — GCC/Clang 拒绝 |
| 修复前（旧 MSVC） | `#define GSL_SUPPRESS(x) [[gsl::suppress(x)]]`   | `[[gsl::suppress(f.6)]]` — 旧 MSVC 能解析 |
| 修复后（推荐） | `#define GSL_SUPPRESS(x) [[gsl::suppress(#x)]]` | `[[gsl::suppress("f.6")]]` — 所有现代编译器可接受 |

## 根因（GCC/Clang 为何拒绝带空格版本）

1. **C++ 属性语法严格**：`[dcl.attr.grammar]` 规定 `(` 必须紧跟在
   attribute-name 之后，中间不能有空白。
2. **词法分析的分裂**：GCC/Clang 遇到 `suppress (f.6)` 时，把
   `suppress` 当作一个“无参数”的未知属性忽略掉，其后的 `(f.6)`
   不再属于该属性，要么被当成无效 token，要么产生语法错误。
3. **非标识符参数的二次问题**：即使去掉空格写成 `[[gsl::suppress(x)]]`，
   当 `x` 是 `f.6`（由多个 token 组成，且不是单个标识符/字面量）时，
   仍无法构成合法的 attribute-argument-clause；因此需要用 `#x`
   stringify 成字符串字面量 `"f.6"`。

## 最小触发用例

位于本目录的 `gsl_suppress_bug.cc`：

```
#define GSL_SUPPRESS(x) [[gsl::suppress (x)]]   // 坏：空格
GSL_SUPPRESS(f.6)
int main() { return 0; }
```

复现：

```
g++ -std=c++17 -Werror -c gsl_suppress_bug.cc
# -> unknown attribute 'suppress' ignored / -Werror=attributes 失败
```

把 `#if 0` 改成 `#if 1` 再编译一次即可看到“坏”宏失败，改回即通过。
