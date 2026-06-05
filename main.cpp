#include <gsl/gsl>
#include <iostream>
#include <string>
#include <vector>

int main()
{
    std::vector<int> data = {10, 20, 30, 40, 50};

    gsl::span<int> sp{data};
    int sum = 0;
    for (auto v : sp) sum += v;

    std::cout << "Sum: " << sum << "\n";

    {
        auto _ = gsl::finally([] { std::cout << "Cleanup via finally\n"; });
        std::cout << "Doing work...\n";
    }

    return 0;
}