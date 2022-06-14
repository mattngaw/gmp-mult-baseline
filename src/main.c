#include <time.h>
#include <stdlib.h>
#include <stdio.h>
#include <gmp.h>
#include "../lib/FFT-Integer-Multiplication/libs/fft.h"

void random_base10(mpz_t rop, gmp_randstate_t state, unsigned long int n) {
    mpz_t max, min, rand;
    mpz_init(max);
    mpz_init(min);
    mpz_init(rand);
    mpz_ui_pow_ui(max, (unsigned long int) 10, n);
    mpz_ui_pow_ui(min, (unsigned long int) 10, n-1);
    mpz_sub(max, max, min);
    mpz_urandomm(rand, state, max);
    mpz_add(rop, rand, min);
}

int main(int argc, char *argv[]) {
    mpz_t *factors1, *factors2, gmp_product, fft_product;
    f64_I *outA, *outB, *twiddle;
    if (argc != 2) {
        printf("Usage: ./main <N>");
        return -1;
    }
    const long N = atoi(argv[1]);
    char *result_buffer;
    long *b4;
    unsigned long seed;
    clock_t t0;

    double total_time_gmp, total_time_fft;

    gmp_randstate_t state;
    gmp_randinit_default(state);
    seed = time(NULL);
    gmp_randseed_ui(state, seed);

    fesetround(FE_UPWARD);
    
    mpz_init(gmp_product);
    mpz_init(fft_product);

    if ((factors1 = malloc(sizeof(mpz_t) * N)) == NULL) {
        printf("Memory allocation failed");
        return -1;
    }

    if ((factors2 = malloc(sizeof(mpz_t) * N)) == NULL) {
        printf("Memory allocation failed");
        return -1;
    }

    for (int k = 0; k < 30; k++) {
        long SIZE = 1 << k;
        printf("Running %ld iterations over factors of 2^%d (%ld) digits...\n", N, k, SIZE);
        total_time_gmp = 0;
        total_time_fft = 0;

        printf("\tGenerating random numbers... ");
        fflush(stdout);
        t0 = clock();
        for (int i = 0; i < N; i++) {
            mpz_init(factors1[i]);
            mpz_init(factors2[i]);
            random_base10(factors1[i], state, SIZE);
            random_base10(factors2[i], state, SIZE);
        }
        printf("Finished in %lf s\n", (double)(clock() - t0) / CLOCKS_PER_SEC);
        if ((outA = malloc(sizeof(f64_I) * 4 * SIZE)) == NULL) {
            printf("Memory allocation failed");
            return -1;
        }

        if ((outB = malloc(sizeof(f64_I) * 4 * SIZE)) == NULL) {
            printf("Memory allocation failed");
            return -1;
        }

        if ((twiddle = malloc(sizeof(f64_I) * 4 * SIZE)) == NULL) {
            printf("Memory allocation failed");
            return -1;
        }

        if ((b4 = malloc(sizeof(long) * 2 * SIZE)) == NULL) {
            printf("Memory allocation failed");
            return -1;
        }

        result_buffer = calloc(sizeof(char), 2 * SIZE);

        printf("\tGenerating twiddles... ");
        fflush(stdout);
        t0 = clock();
        initialize_twiddles(2 * SIZE, twiddle);
        printf("Finished in %lf s\n", (double)(clock() - t0) / CLOCKS_PER_SEC);

        int num_incorrect = 0;

        printf("\tRunning %d multiplications... ", N);
        fflush(stdout);
        for (int i = 0; i < N; i++) {
            readinit(outA, 4, SIZE, mpz_get_str(NULL, 10, factors1[i]));
            readinit(outB, 4, SIZE, mpz_get_str(NULL, 10, factors2[i]));

            t0 = clock();
            int prod = int_mult(SIZE, outA, outB, b4, twiddle, result_buffer);
            total_time_fft += (double)(clock() - t0) / CLOCKS_PER_SEC;

            mpz_init_set_str(fft_product, result_buffer, 10);

            t0 = clock();
            mpz_mul(gmp_product, factors1[i], factors2[i]);
            total_time_gmp += (double)(clock() - t0) / CLOCKS_PER_SEC;

            if (mpz_cmp(fft_product, gmp_product) != 0) {
                gmp_printf("%Zd, %Zd, %Zd, %Zd, %s", factors1[i], factors2[i], gmp_product, fft_product, result_buffer);
                printf("FAILURE: Incorrect result at SIZE = 2^%d\n", k);
                return -1;
            }
        }
        printf("Finished!\n");

        free(outA);
        free(outB);
        free(b4);
        free(twiddle);
        free(result_buffer);


        printf("\tavg_time_gmp = %lf\n", total_time_gmp / (double)N);
        printf("\tavg_time_fft = %lf\n", total_time_fft / (double)N);
    }
    mpz_clear(gmp_product);
    mpz_clear(fft_product);
    return 0;
}