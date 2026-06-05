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

#include <gsl/assert>
#include <gtest/gtest.h>

// Test that GSL_SUPPRESS macro is defined
TEST(gsl_suppress_tests, macro_is_defined)
{
#ifdef GSL_SUPPRESS
    EXPECT_TRUE(true);
#else
    FAIL() << "GSL_SUPPRESS macro should be defined";
#endif
}

// Test that GSL_SUPPRESS_USE_QUOTES is correctly set per compiler
TEST(gsl_suppress_tests, use_quotes_detection)
{
#if defined(__clang__)
    EXPECT_EQ(GSL_SUPPRESS_USE_QUOTES, 1);
#elif defined(_MSC_VER) && _MSC_VER >= 1950
    EXPECT_EQ(GSL_SUPPRESS_USE_QUOTES, 1);
#else
    EXPECT_EQ(GSL_SUPPRESS_USE_QUOTES, 0);
#endif
}

// Test that GSL_HAS_SUPPRESS_ATTRIBUTE detection works
TEST(gsl_suppress_tests, has_suppress_attribute_detection)
{
#if defined(__has_cpp_attribute)
#if __has_cpp_attribute(gsl::suppress)
    EXPECT_EQ(GSL_HAS_SUPPRESS_ATTRIBUTE, 1);
#else
    EXPECT_EQ(GSL_HAS_SUPPRESS_ATTRIBUTE, 0);
#endif
#else
    EXPECT_EQ(GSL_HAS_SUPPRESS_ATTRIBUTE, 0);
#endif
}

// Test GSL_SUPPRESS with unquoted tag (standard usage)
// This should compile without errors on all supported compilers
TEST(gsl_suppress_tests, unquoted_tag_usage)
{
    // The macro should expand to a valid attribute or nothing
    // We just verify it compiles
    auto func = []() GSL_SUPPRESS(f.4) {
        return 42;
    };
    EXPECT_EQ(func(), 42);
}

// Test GSL_SUPPRESS with explicit quoted tag
// This tests the new capability of supporting both forms
TEST(gsl_suppress_tests, quoted_tag_usage)
{
    // Explicit string form should also work
    auto func = []() GSL_SUPPRESS("f.4") {
        return 42;
    };
    EXPECT_EQ(func(), 42);
}

// Test GSL_SUPPRESS on a function declaration
GSL_SUPPRESS(f.6)
void test_function_with_suppress()
{
    // This function intentionally does nothing
}

TEST(gsl_suppress_tests, function_declaration_suppress)
{
    // Just verify the function can be called
    test_function_with_suppress();
    EXPECT_TRUE(true);
}

// Test GSL_SUPPRESS on a variable
TEST(gsl_suppress_tests, variable_suppress)
{
    GSL_SUPPRESS(es.11)
    int arr[10] = {0};
    EXPECT_EQ(arr[0], 0);
}

// Test GSL_SUPPRESS with different tag formats
TEST(gsl_suppress_tests, various_tag_formats)
{
    // Single tag
    auto f1 = []() GSL_SUPPRESS(bounds.1) { return 1; };
    EXPECT_EQ(f1(), 1);

    // Type-related tag
    auto f2 = []() GSL_SUPPRESS(type.1) { return 2; };
    EXPECT_EQ(f2(), 2);

    // Owner-related tag
    auto f3 = []() GSL_SUPPRESS(owner.1) { return 3; };
    EXPECT_EQ(f3(), 3);
}

// Test that GSL_SUPPRESS doesn't break C++14 compatibility
TEST(gsl_suppress_tests, cpp14_compatibility)
{
#if __cplusplus >= 201402L
    // Verify we're compiling with at least C++14
    auto lambda = []() GSL_SUPPRESS(f.4) {
        return 14;
    };
    EXPECT_EQ(lambda(), 14);
#else
    GTEST_SKIP() << "C++14 or later required";
#endif
}

// Test strict mode compile definition
TEST(gsl_suppress_tests, strict_mode_definition)
{
#ifdef GSL_SUPPRESS_STRICT_MODE
    EXPECT_EQ(GSL_SUPPRESS_STRICT_MODE, 1);
#else
    EXPECT_EQ(GSL_SUPPRESS_STRICT_MODE, 0);
#endif
}

// Test that GSL_SUPPRESS_IMPL is correctly defined
TEST(gsl_suppress_tests, impl_macro_defined)
{
#ifdef GSL_SUPPRESS_IMPL
    EXPECT_TRUE(true);
#else
    FAIL() << "GSL_SUPPRESS_IMPL should be defined";
#endif
}

// Test multiple suppressions on same scope
TEST(gsl_suppress_tests, multiple_suppressions)
{
    GSL_SUPPRESS(bounds.1)
    GSL_SUPPRESS(type.4)
    int arr[5] = {1, 2, 3, 4, 5};
    EXPECT_EQ(arr[2], 3);
}

// Test GSL_SUPPRESS in class method
class TestClass
{
public:
    GSL_SUPPRESS(c.12)
    int getValue() const
    {
        return value;
    }

    GSL_SUPPRESS(f.6)
    void setValue(int v)
    {
        value = v;
    }

private:
    int value = 0;
};

TEST(gsl_suppress_tests, class_method_suppress)
{
    TestClass obj;
    obj.setValue(42);
    EXPECT_EQ(obj.getValue(), 42);
}

// Test that suppressions work with template functions
template<typename T>
GSL_SUPPRESS(t.1)
T template_func(T t)
{
    return t;
}

TEST(gsl_suppress_tests, template_function_suppress)
{
    EXPECT_EQ(template_func(42), 42);
    EXPECT_EQ(template_func(3.14), 3.14);
}
