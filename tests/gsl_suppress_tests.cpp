#include <gsl/assert>
#include <gsl/span>

#include <gtest/gtest.h>

TEST(gsl_suppress_tests, suppress_26481_pointer_arithmetic)
{
    int arr[10] = {};
    gsl::span<int> s{arr};

    GSL_SUPPRESS(26481)
    int* p = arr + 1;

    EXPECT_EQ(*p, 0);
    (void)s;
}
