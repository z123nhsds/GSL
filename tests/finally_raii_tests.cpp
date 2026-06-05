#include <gtest/gtest.h>
#include <gsl/util>
#include <stdexcept>
#include <string>
#include <vector>

#include "gsl_finally_raii.h"

using namespace gsl;

TEST(finally_raii, basic_cleanup_on_scope_exit)
{
    int counter = 0;
    {
        auto _ = finally_safe([&] { ++counter; });
        EXPECT_EQ(counter, 0);
    }
    EXPECT_EQ(counter, 1);
}

TEST(finally_raii, move_semantics_transfer_ownership)
{
    int counter = 0;
    {
        auto guard_1 = finally_safe([&] { ++counter; });
        {
            auto guard_2 = std::move(guard_1);
            EXPECT_EQ(counter, 0);
        }
        EXPECT_EQ(counter, 1);
        {
            auto guard_3 = std::move(guard_1);
            EXPECT_EQ(counter, 1);
        }
        EXPECT_EQ(counter, 1);
    }
    EXPECT_EQ(counter, 1);
}

TEST(finally_raii, dismiss_prevents_cleanup)
{
    int counter = 0;
    {
        auto guard = finally_safe([&] { ++counter; });
        guard.dismiss();
    }
    EXPECT_EQ(counter, 0);
}

TEST(finally_raii, cleanup_runs_during_exception_unwinding)
{
    std::vector<int> log;

    try {
        auto _ = finally_safe([&] { log.push_back(1); });
        log.push_back(0);
        throw std::runtime_error("test exception");
    } catch (const std::runtime_error&) {
        log.push_back(2);
    }

    ASSERT_EQ(log.size(), 3);
    EXPECT_EQ(log[0], 0);
    EXPECT_EQ(log[1], 1);
    EXPECT_EQ(log[2], 2);
}

TEST(finally_raii, cleanup_function_throws_no_terminate)
{
    int counter = 0;
    {
        auto _ = finally_safe([&] {
            ++counter;
            throw std::runtime_error("cleanup failed");
        });
    }
    EXPECT_EQ(counter, 1);
}

TEST(finally_raii, cleanup_throws_during_stack_unwinding)
{
    int counter = 0;

    try {
        auto _ = finally_safe([&] {
            ++counter;
            throw std::runtime_error("cleanup throws");
        });
        throw std::runtime_error("primary exception");
    } catch (const std::runtime_error& e) {
        EXPECT_EQ(std::string(e.what()), "primary exception");
    }

    EXPECT_EQ(counter, 1);
}

TEST(finally_raii, nested_exception_with_cleanup)
{
    int outer = 0;
    int inner = 0;

    try {
        auto _outer = finally_safe([&] { outer = 42; });

        try {
            auto _inner = finally_safe([&] { inner = 42; });
            throw std::runtime_error("inner");
        } catch (const std::runtime_error&) {
            EXPECT_EQ(inner, 42);
        }

        throw std::runtime_error("outer");
    } catch (const std::runtime_error&) {
        EXPECT_EQ(outer, 42);
    }
}

TEST(finally_raii, multiple_cleanup_guards_reverse_order)
{
    std::vector<int> order;

    {
        auto g1 = finally_safe([&] { order.push_back(1); });
        auto g2 = finally_safe([&] { order.push_back(2); });
        auto g3 = finally_safe([&] { order.push_back(3); });
    }

    ASSERT_EQ(order.size(), 3);
    EXPECT_EQ(order[0], 3);
    EXPECT_EQ(order[1], 2);
    EXPECT_EQ(order[2], 1);
}

TEST(finally_raii, function_pointer)
{
    int counter = 0;
    auto inc = [&] { ++counter; };

    {
        auto _ = finally_safe(+inc);
    }
    EXPECT_EQ(counter, 1);
}

TEST(finally_raii, comparable_to_std_final_action)
{
    int a = 0;
    int b = 0;

    {
        auto _safe = finally_safe([&] { a = 1; });
        auto _std  = finally([&] { b = 1; });
    }

    EXPECT_EQ(a, 1);
    EXPECT_EQ(b, 1);
}