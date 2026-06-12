// fixed.cc
// 修复后的宏：`suppress` 与 `(` 紧贴，且 `#x` stringify 参数。
// 编译：g++ -std=c++17 -Werror -c fixed.cc   -> 应 PASS
//       clang++ -std=c++17 -Werror -c fixed.cc -> 应 PASS

#define GSL_SUPPRESS(x) [[gsl::suppress(#x)]]

GSL_SUPPRESS(f.6)
int f(int* p) { return p ? *p : 0; }

GSL_SUPPRESS(type.1)
int main() { return f((int*)0); }
