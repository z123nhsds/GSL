// trigger.cc
// 仅含“修复前”的空格版本宏，用来在 GCC/Clang 下复现编译失败。
// 编译：g++ -std=c++17 -Werror -c trigger.cc   -> 应 FAIL
//       clang++ -std=c++17 -Werror -c trigger.cc -> 应 FAIL

#define GSL_SUPPRESS(x) [[gsl::suppress (x)]]

GSL_SUPPRESS(f.6)
int f(int* p) { return p ? *p : 0; }

GSL_SUPPRESS(type.1)
int main() { return f((int*)0); }
