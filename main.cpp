#include <gsl/gsl>
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
    void release() noexcept
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
    std::printf("\n=== Demo: Normal Exit (gsl::finally) ===\n");
    Resource res{1};
    auto cleanup = gsl::finally([&res]() noexcept { res.release(); });
    res.use();
}

void demo_exception_exit()
{
    std::printf("\n=== Demo: Exception Exit (gsl::finally) ===\n");
    Resource res{2};

    try
    {
        auto cleanup = gsl::finally([&res]() noexcept { res.release(); });
        res.use();
        throw std::runtime_error("Something went wrong");
    }
    catch (const std::exception& e)
    {
        std::printf("[catch] Exception: %s (resource already released)\n", e.what());
    }
}

int main()
{
    demo_normal_exit();
    demo_exception_exit();
    std::printf("\nAll demos completed.\n");
    return 0;
}