///////////////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2015 Microsoft Corporation. All rights reserved.
//
// This code is licensed under the MIT License (MIT).
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
// THE SOFTWARE.
//
///////////////////////////////////////////////////////////////////////////////

#include <gtest/gtest.h>

#include <gsl/assert>
#include <gsl/util>

using namespace gsl;

namespace
{

// Test function demonstrating both forms of GSL_SUPPRESS
GSL_SUPPRESS(type.1)
int test_function_with_identifier_form(int x)
{
    return x;
}

// Test function demonstrating GSL_SUPPRESS_STR with string literal
GSL_SUPPRESS_STR("type.1")
int test_function_with_string_form(int x)
{
    return x;
}

// Test that both macros compile and work together
GSL_SUPPRESS(bounds.1)
GSL_SUPPRESS_STR("type.1")
int test_multiple_suppressions(int x, const int* arr)
{
    return arr[x];
}

} // namespace

TEST(gsl_suppress_tests, identifier_form_compiles)
{
    int result = test_function_with_identifier_form(42);
    EXPECT_EQ(result, 42);
}

TEST(gsl_suppress_tests, string_form_compiles)
{
    int result = test_function_with_string_form(42);
    EXPECT_EQ(result, 42);
}

TEST(gsl_suppress_tests, multiple_suppressions_compile)
{
    int arr[] = {1, 2, 3};
    int result = test_multiple_suppressions(0, arr);
    EXPECT_EQ(result, 1);
}

// Verify that GSL_SUPPRESS_STRINGIFY works correctly
TEST(gsl_suppress_tests, stringify_macro_works)
{
    // This test just verifies that the stringify macro compiles
    // by using it in a simple context
    const char* str = GSL_SUPPRESS_STRINGIFY(test);
    EXPECT_STREQ(str, "test");
}