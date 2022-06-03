#include <time.h>
#include <stdio.h>
#include <gmp.h>

int main() {
    // Initialize state and seed for random numbers
    gmp_randstate_t state;
    mpz_t seed;
    gmp_randinit_default(state);
    mpz_init_set_ui(seed, (unsigned long int)clock());

    // Initialize factors (x, y) and result (z)
    mpz_t x, y, z;
    mpz_init(x);
    mpz_init(y);
    mpz_init(z);

    // Constants
    const mp_bitcnt_t FACTOR_SIZE = 64 * 0x100000;
    const int NUM_ITERATIONS = 1;

    // Open output file
    FILE *out = fopen("products.out", "a+");

    // Initialize
    clock_t start, end;
    double cpu_time, total_cpu_time;
    total_cpu_time = 0;

    for (int i = 0; i < NUM_ITERATIONS; i++) {
        mpz_urandomb(x, state, FACTOR_SIZE);
        mpz_urandomb(y, state, FACTOR_SIZE);
        start = clock();
        mpz_mul(z, y, x);
        end = clock();
        cpu_time = ((double) (end - start)) / CLOCKS_PER_SEC;
        total_cpu_time += cpu_time;
        gmp_fprintf(out, "%Zd\n", z);
    }

    printf("FACTOR_SIZE = %ld \n", FACTOR_SIZE);
    printf("NUM_ITERATIONS = %d \n", NUM_ITERATIONS);
    printf("%lf sec\n", total_cpu_time);

    // Close output file
    fclose(out);

    return 0;
}
