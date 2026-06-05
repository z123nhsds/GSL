#ifndef GSL_FINALLY_RAII_H
#define GSL_FINALLY_RAII_H

#include <gsl/util>
#include <exception>
#include <type_traits>
#include <utility>

namespace gsl {

template <class F>
class final_action_safe
{
public:
    static_assert(std::is_nothrow_move_constructible_v<F> || std::is_copy_constructible_v<F>,
        "The callable must be nothrow move constructible or copy constructible");

    explicit final_action_safe(const F& ff) noexcept(std::is_nothrow_copy_constructible_v<F>)
        : f{ff} { }
    explicit final_action_safe(F&& ff) noexcept(std::is_nothrow_move_constructible_v<F>)
        : f{std::move(ff)} { }

    ~final_action_safe() noexcept
    {
        if (!invoke) return;
#if defined(__cpp_exceptions)
        try {
            f();
        } catch (...) {
        }
#else
        f();
#endif
    }

    final_action_safe(final_action_safe&& other) noexcept
        : f(std::move(other.f)), invoke(std::exchange(other.invoke, false))
    { }

    final_action_safe(const final_action_safe&)   = delete;
    void operator=(const final_action_safe&) = delete;
    void operator=(final_action_safe&&)      = delete;

    void dismiss() noexcept { invoke = false; }

private:
    F f;
    bool invoke = true;
};

template <class F>
GSL_NODISCARD auto finally_safe(F&& f) noexcept
{
    return final_action_safe<std::decay_t<F>>{std::forward<F>(f)};
}

} // namespace gsl

#endif // GSL_FINALLY_RAII_H