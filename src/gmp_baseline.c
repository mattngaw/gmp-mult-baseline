#include <time.h>
#include <stdio.h>
#include <gmp.h>

int main(int argc, char *argv[]) {
    // Initialize factors (x, y), result (z), and input strings (x_str, y_str)
    mpz_t x, y, z;
    mpz_init(x);
    mpz_init(y);
    mpz_init(z);
    char *x_str, *y_str;

    if (argc == 3) { // Grab input numbers
        x_str = argv[1];
        y_str = argv[2];
    } else { // Otherwise input error
        fprintf(stderr, "Invalid number of arguments: %d", argc);
        return 1;
    }

    // Set x and y
    mpz_set_str(x, x_str, 10);
    mpz_set_str(y, y_str, 10);

    // Multiply
    mpz_mul(z, y, x);
    gmp_printf("%Zd\n", z);

    return 0;
}
