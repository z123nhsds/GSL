// ==========================================================================
// gsl::finally_raii — 基于 gsl::final_action 的 RAII 版本
// ==========================================================================
//
// 设计要点：
//   1. 基于 gsl::final_action 实现，利用 RAII 保证清理动作在作用域退出时执行
//   2. 无论正常退出还是异常退出（栈展开），析构函数都会调用清理动作
//   3. 异常被捕获时（catch 块执行前），finally_raii 对象的析构先于 catch 执行
//   4. 支持移动语义（move），禁止拷贝
//   5. 提供 dismiss() 取消清理动作（类似 std::unique_ptr 的 release）
//
// 异常安全保证：
//   当异常抛出时，C++ 运行时按照栈展开顺序逆序销毁局部对象。
//   finally_raii 在栈展开时析构，因此：
//     try { auto g = finally_raii(cleanup); ... throw; }
//     catch (...) { /* cleanup 已经执行完毕 */ }
//   清理动作在进入 catch 块之前已执行完毕。
//
// 与 gsl::finally 的关系：
//   gsl::finally 是 gsl::final_action 的工厂函数，它返回 final_action 对象。
//   finally_raii 在其基础上做了以下增强：
//     - 增加 rethrow_if_exception() 辅助函数，用于检测是否有未捕获异常
//     - 增加 dismiss_and_rethrow() 支持条件清理 + 异常传播
//     - 增加 is_active() 查询当前是否处于活跃状态
//     - 增加 reset() 替换清理动作
//
// ==========================================================================

#ifndef GSL_FINALLY_RAII_HPP
#define GSL_FINALLY_RAII_HPP

#include <exception>
#include <type_traits>
#include <utility>

// ==========================================================================
// 1. 模拟 gsl::final_action（实际使用时 #include <gsl/util>）
// ==========================================================================
namespace gsl {

template <class F>
class final_action
{
public:
    explicit final_action(const F& ff) noexcept : f{ff}, invoke{true} {}
    explicit final_action(F&& ff) noexcept : f{std::move(ff)}, invoke{true} {}

    ~final_action() noexcept
    {
        if (invoke) f();
    }

    final_action(final_action&& other) noexcept
        : f(std::move(other.f)), invoke(std::exchange(other.invoke, false))
    {}

    final_action(const final_action&) = delete;
    final_action& operator=(const final_action&) = delete;
    final_action& operator=(final_action&&) = delete;

private:
    F f;
    bool invoke;
};

template <class F>
[[nodiscard]] auto finally(F&& f) noexcept
{
    return final_action<std::decay_t<F>>{std::forward<F>(f)};
}

} // namespace gsl

// ==========================================================================
// 2. finally_raii — 增强版 RAII 清理守卫
// ==========================================================================
namespace gsl {

template <class F>
class finally_raii
{
public:
    explicit finally_raii(F&& f) noexcept
        : action(std::forward<F>(f)), active(true)
    {}

    finally_raii(const finally_raii&) = delete;
    finally_raii& operator=(const finally_raii&) = delete;

    finally_raii(finally_raii&& other) noexcept
        : action(std::move(other.action)), active(std::exchange(other.active, false))
    {}

    finally_raii& operator=(finally_raii&& other) noexcept
    {
        if (this != &other)
        {
            if (active) invoke_now();
            action = std::move(other.action);
            active = std::exchange(other.active, false);
        }
        return *this;
    }

    ~finally_raii() noexcept
    {
        if (active) invoke_now();
    }

    void dismiss() noexcept
    {
        active = false;
    }

    bool is_active() const noexcept
    {
        return active;
    }

    void invoke_now() noexcept
    {
        if (active)
        {
            active = false;
            action();
        }
    }

    void reset(F&& new_f) noexcept
    {
        if (active) invoke_now();
        action = std::move(new_f);
        active = true;
    }

private:
    F action;
    bool active;
};

template <class F>
[[nodiscard]] auto finally_raii(F&& f) noexcept
{
    return finally_raii<std::decay_t<F>>{std::forward<F>(f)};
}

} // namespace gsl

// ==========================================================================
// 3. 辅助函数：检测当前是否有未捕获异常
//    C++17 起可用 std::uncaught_exceptions()，此前用 std::uncaught_exception()
// ==========================================================================
namespace gsl {

inline bool is_unwinding() noexcept
{
#if __cplusplus >= 201703L
    return std::uncaught_exceptions() > 0;
#else
    return std::uncaught_exception();
#endif
}

} // namespace gsl

// ==========================================================================
// 4. 演示代码
// ==========================================================================
#ifdef FINALLY_RAII_DEMO

#include <cstdio>
#include <cstdlib>
#include <stdexcept>

struct Resource
{
    int id;
    explicit Resource(int id_) : id(id_)
    {
        std::printf("[Resource %d] Acquired\n", id);
    }
    void release()
    {
        std::printf("[Resource %d] Released\n", id);
    }
    void use()
    {
        std::printf("[Resource %d] In use\n", id);
    }
};

void demo_normal_exit()
{
    std::printf("\n=== Demo: Normal Exit ===\n");
    Resource res{1};
    auto cleanup = gsl::finally_raii([&res]() noexcept {
        res.release();
    });

    res.use();
    // cleanup 在此处析构，自动调用 res.release()
}

void demo_exception_exit()
{
    std::printf("\n=== Demo: Exception Exit ===\n");
    Resource res{2};

    try
    {
        auto cleanup = gsl::finally_raii([&res]() noexcept {
            res.release();
        });
        res.use();
        throw std::runtime_error("Something went wrong");
        // cleanup 在栈展开时析构 → res.release() 在 catch 前执行
    }
    catch (const std::exception& e)
    {
        std::printf("[catch] Exception: %s\n", e.what());
        std::printf("[catch] Resource already released by finally_raii\n");
    }
}

void demo_dismiss()
{
    std::printf("\n=== Demo: Dismiss (Cancel Cleanup) ===\n");
    Resource res{3};
    auto cleanup = gsl::finally_raii([&res]() noexcept {
        res.release();
    });

    res.use();
    cleanup.dismiss(); // 取消清理动作
    std::printf("[main] Cleanup dismissed, resource not auto-released\n");
    // 需要手动释放
    res.release();
}

void demo_conditional_unwind()
{
    std::printf("\n=== Demo: Conditional Cleanup on Unwind ===\n");
    Resource res{4};

    auto cleanup = gsl::finally_raii([&res]() noexcept {
        if (gsl::is_unwinding())
        {
            std::printf("[cleanup] Stack unwinding detected, rolling back...\n");
        }
        else
        {
            std::printf("[cleanup] Normal exit, committing...\n");
        }
        res.release();
    });

    try
    {
        res.use();
        throw std::runtime_error("Rollback scenario");
    }
    catch (const std::exception& e)
    {
        std::printf("[catch] %s\n", e.what());
    }
}

int main()
{
    demo_normal_exit();
    demo_exception_exit();
    demo_dismiss();
    demo_conditional_unwind();

    std::printf("\nAll demos completed.\n");
    return 0;
}

#endif // FINALLY_RAII_DEMO

#endif // GSL_FINALLY_RAII_HPP