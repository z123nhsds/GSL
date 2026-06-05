#define GSL_SUPPRESS(x) [[gsl::suppress(x)]]

GSL_SUPPRESS(f .4)
void test() {}

int main() { return 0; }
