#include <gsl/assert>

int main()
{
    int values[]{1, 2, 3};
    int* begin = &values[0];

    GSL_SUPPRESS(26481)
    {
        return *(begin + 1) != 2;
    }
}
