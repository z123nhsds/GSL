// gsl_suppress_bug.cc
// ---------------------------------------------------------------------------
// 最小触发用例：演示 `GSL_SUPPRESS` 宏在 `suppress` 与 `(` 之间留有空格时，
// GCC/Clang 会拒绝编译（或发出 -Werror 的错误）。
//
// 原宏定义：
//     #define GSL_SUPPRESS(x) [[gsl::suppress (x)]]
//                         注意这里 ^ 的空格
//
// 修复后的宏定义：
//     #define GSL_SUPPRESS(x) [[gsl::suppress(#x)]]
//     或（仅旧版 MSVC 可接受，且必须紧跟 `(`）：
//     #define GSL_SUPPRESS(x) [[gsl::suppress(x)]]
//
// 根因：
//   C++11 属性的语法为 `[[ attribute-list ]]`，其中每个 attribute 形如
//       attribute-namespace::attribute-name(opt(argument-clause))
//   标准（[dcl.attr.grammar]）要求 `(` 必须紧跟 attribute-name，二者之间
//   不能出现空白。GCC 与 Clang 的前端严格按此规则做词法/语法分析：
//     - `[[gsl::suppress("f.6")]]`  -> 合法
//     - `[[gsl::suppress ("f.6")]]` -> `suppress` 被当作无参数的属性，
//                                      紧随其后的 `("f.6")` 被当作独立
//                                      的 token 序列，编译器无法识别并报
//                                      “unknown attribute 'suppress' ignored”
//                                      或直接语法错误。
//   当参数 `x` 形如 `f.6` 这种由多 token 组成、且无法构成单个标识符/
//   字符串字面量时，必须使用 `#x` 把它 stringify 成一个 string-literal，
//   才能被属性语法接受（否则就是裸的 `f.6`，同样会失败）。
//
// 复现命令（任选其一即会失败，用 `#` 版本即可通过）：
//     g++ -std=c++17 -Werror -c gsl_suppress_bug.cc
//     clang++ -std=c++17 -Werror -c gsl_suppress_bug.cc
// ---------------------------------------------------------------------------

#if 0
// ---- 修复前的“坏”宏：`suppress` 与 `(` 之间有空格。
//      GCC/Clang 会报错：
//          warning: unknown attribute 'suppress' ignored
//          error: expected expression before ')' token  （视参数而定）
//          或直接 -Werror 将 warning 升级为 error。
#define GSL_SUPPRESS(x) [[gsl::suppress (x)]]
#else
// ---- 修复后的“好”宏：`suppress` 紧跟 `(`，且通过 `#x` stringify 参数。
#define GSL_SUPPRESS(x) [[gsl::suppress(#x)]]
#endif

GSL_SUPPRESS(f.6)
int f(int* p) { return p ? *p : 0; }

GSL_SUPPRESS(type.1)
int main() { return f((int*)0); }
