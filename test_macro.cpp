#define GSL_SUPPRESS(x) [[gsl::suppress(x)]]
int main() {
    GSL_SUPPRESS(bounds.1)
    int a = 0;
    return a;
}
