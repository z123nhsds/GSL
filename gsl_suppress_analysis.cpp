// ==========================================================================
// GSL_SUPPRESS 宏在 MSVC 与 GCC 下的不同展开形式分析
// 基于 commit: f3c5967 - "remove space in GSL_SUPPRESS" (PR #1248)
// ==========================================================================
//
// 一、GSL_SUPPRESS 宏定义（来源: include/gsl/assert）
//
//   #if defined(__clang__)
//   #define GSL_SUPPRESS(x) [[gsl::suppress(#x)]]
//   #elif defined(_MSC_VER) && _MSC_VER >= 1950
//   #define GSL_SUPPRESS(x) [[gsl::suppress(#x)]]
//   #elif defined(_MSC_VER) && !defined(__INTEL_COMPILER) && !defined(__NVCC__)
//   #define GSL_SUPPRESS(x) [[gsl::suppress(x)]]
//   #else
//   #define GSL_SUPPRESS(x)
//   #endif
//
// ==========================================================================
//
// 二、各编译器下展开形式对比
//
//   ┌─────────────────────┬──────────────────────────────────────────────┐
//   │ 编译器                │ GSL_SUPPRESS(bounds.1) 展开结果               │
//   ├─────────────────────┼──────────────────────────────────────────────┤
//   │ Clang                │ [[gsl::suppress("bounds.1")]]               │
//   │ MSVC >= 1950 (2022+) │ [[gsl::suppress("bounds.1")]]               │
//   │ MSVC < 1950 (旧版)    │ [[gsl::suppress(bounds.1)]]                 │
//   │ GCC (及其他)          │ (空 — 不产生任何代码)                         │
//   └─────────────────────┴──────────────────────────────────────────────┘
//
// 关键差异：
// 1. Clang / 新版 MSVC: 使用 #x 将参数字符串化，生成带引号的 tag 如 "bounds.1"
// 2. 旧版 MSVC: 直接展开参数，不字符串化，生成不带引号的 tag 如 bounds.1
// 3. GCC: 宏展开为空，GCC 不支持 [[gsl::suppress]] 属性语法
//
// ==========================================================================
//
// 三、原 Bug 分析 — 宏参数中多余空格导致编译失败
//
// 原代码（Bug 版本）：
//   GSL_SUPPRESS(bounds .1)    // 注意 "bounds" 和 ".1" 之间有空格
//   GSL_SUPPRESS(f .4)          // 注意 "f" 和 ".4" 之间有空格
//   GSL_SUPPRESS(f .23)         // 注意 "f" 和 ".23" 之间有空格
//
// 修复后（PR #1248）：
//   GSL_SUPPRESS(bounds.1)     // 无空格
//   GSL_SUPPRESS(f.4)           // 无空格
//   GSL_SUPPRESS(f.23)          // 无空格
//
// 编译失败原因分析（分编译器）：
//
// ┌─ Clang / 新版 MSVC (_MSC_VER >= 1950) ──────────────────────────────┐
// │                                                                      │
// │   GSL_SUPPRESS(bounds .1)                                            │
// │   → [[gsl::suppress("bounds .1")]]                                    │
// │                                                                      │
// │   字符串化后得到 "bounds .1"，其中包含空格，导致 Clang/MSVC 将          │
// │   "bounds" 和 ".1" 解析为两个独立的 token，不是合法的 C++ Core         │
// │   Guidelines 规则标签，触发编译错误。                                  │
// │                                                                      │
// │   正确形式：[[gsl::suppress("bounds.1")]]                             │
// │                                                                      │
// └──────────────────────────────────────────────────────────────────────┘
//
// ┌─ 旧版 MSVC (_MSC_VER < 1950) ──────────────────────────────────────┐
// │                                                                      │
// │   GSL_SUPPRESS(bounds .1)                                            │
// │   → [[gsl::suppress(bounds .1)]]                                     │
// │                                                                      │
// │   属性参数中 bounds 和 .1 被空格分隔，MSVC 属性解析器无法识别           │
// │   "bounds .1" 作为有效的规则标签，编译失败。                           │
// │                                                                      │
// └──────────────────────────────────────────────────────────────────────┘
//
// ┌─ GCC ───────────────────────────────────────────────────────────────┐
// │                                                                      │
// │   GSL_SUPPRESS(bounds .1)                                            │
// │   → (空)                                                             │
// │                                                                      │
// │   GCC 将宏展开为空，不产生任何代码。但 C++ 预处理器在解析宏参数时，    │
// │   仍会将 "bounds .1" 作为一个整体 token 序列传递给宏参数 x。          │
// │   虽然在 GCC 上不会直接报错（因为宏体为空），但无法起到抑制警告的     │
// │   作用，且代码意图不清晰。                                            │
// │                                                                      │
// └──────────────────────────────────────────────────────────────────────┘
//
// ==========================================================================
//
// 四、涉及修改的文件（commit f3c5967）
//
//   include/gsl/gsl_assert → 修复 GSL_SUPPRESS(bounds.1) 中的空格
//   include/gsl/gsl_util   → 无需修改（该文件已使用正确格式）
//   tests/notnull_tests.cpp → 修复 GSL_SUPPRESS(f.4) 中的空格
//   tests/owner_tests.cpp   → 修复 GSL_SUPPRESS(f.23) 中的空格
//
// ==========================================================================

// 以下为演示代码，展示正确用法：

#include <cstdio>

// 模拟 GSL_SUPPRESS（实际使用时包含 <gsl/assert>）
#if defined(__clang__)
#define GSL_SUPPRESS(x) [[gsl::suppress(#x)]]
#elif defined(_MSC_VER) && _MSC_VER >= 1950
#define GSL_SUPPRESS(x) [[gsl::suppress(#x)]]
#elif defined(_MSC_VER) && !defined(__INTEL_COMPILER) && !defined(__NVCC__)
#define GSL_SUPPRESS(x) [[gsl::suppress(x)]]
#else
#define GSL_SUPPRESS(x)
#endif

int main()
{
    // 正确用法：tag 中无空格
    GSL_SUPPRESS(bounds.1)
    int arr[10];
    int* p = arr + 1;

    // 错误用法（会导致编译失败）：
    // GSL_SUPPRESS(bounds .1)  // 空格导致 tag 解析失败

    (void)p;
    std::printf("GSL_SUPPRESS analysis complete.\n");
    return 0;
}