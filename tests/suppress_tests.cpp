#include <gsl/gsl>
#include <gtest/gtest.h>

namespace
{
GSL_SUPPRESS(test.gsl.suppress)
int suppress_probe(int value)
{
    return value + 1;
}

#if GSL_SUPPRESS_STRICT_MODE && !GSL_SUPPRESS_IS_SUPPORTED
[[gsl::suppress("unused.gsl.suppress")]]
int strict_mode_probe(int value)
{
    return value - 1;
}
#elif GSL_SUPPRESS_STRICT_MODE
GSL_SUPPRESS(unused.gsl.suppress)
int strict_mode_probe(int value)
{
    return value - 1;
}
#endif
} // namespace

TEST(suppress_tests, compiler_selection_matches_expected_form)
{
#if defined(__clang__)
    EXPECT_EQ(1, GSL_SUPPRESS_USES_STRING_LITERAL);
#elif defined(_MSC_VER) && !defined(__INTEL_COMPILER) && !defined(__NVCC__) && (_MSC_VER >= 1950)
    EXPECT_EQ(1, GSL_SUPPRESS_USES_STRING_LITERAL);
#elif defined(_MSC_VER) && !defined(__INTEL_COMPILER) && !defined(__NVCC__)
    EXPECT_EQ(0, GSL_SUPPRESS_USES_STRING_LITERAL);
#else
    EXPECT_EQ(1, GSL_SUPPRESS_USES_STRING_LITERAL);
#endif
}

TEST(suppress_tests, compiler_support_matches_expected_probe)
{
#if defined(__clang__)
    EXPECT_EQ(1, GSL_SUPPRESS_IS_SUPPORTED);
#elif defined(_MSC_VER) && !defined(__INTEL_COMPILER) && !defined(__NVCC__)
    EXPECT_EQ(1, GSL_SUPPRESS_IS_SUPPORTED);
#else
    EXPECT_EQ(0, GSL_SUPPRESS_IS_SUPPORTED);
#endif
}

TEST(suppress_tests, strict_mode_flag_is_available_to_tests)
{
#if GSL_SUPPRESS_STRICT_MODE
    EXPECT_EQ(0, strict_mode_probe(1));
#else
    SUCCEED();
#endif

    EXPECT_EQ(2, suppress_probe(1));
}
