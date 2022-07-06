#include <ctype.h>
#include <time.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <gmp.h>
#include "../lib/FFT-Integer-Multiplication/libs/fft.h"
#include <igen_lib_scalar.h>
#include <igen_math.h>
#include <igen_dd_casts.h>
#include <igen_dd_lib_scalar.h>
#include <omp.h>

/********************
 * HELPER FUNCTIONS *
 ********************/

bool str_is_int(char *str) {
    int len = strlen(str);
    for (int i = 0; i < len; i++) {
        if (!isdigit(str[i])) { return false; }
    }
    return true;
}

void random_factors(mpz_t *factors1, mpz_t *factors2, 
                    long size, long bitwidth,
                    gmp_randstate_t state, int N)
{
    mpz_t f1, f2, base, max, min, diff;
    mpz_init(f1);
    mpz_init(f2);
    mpz_init_set_ui(base, 1 << bitwidth);
    mpz_init(max);
    mpz_init(min);
    mpz_init(diff);
    mpz_pow_ui(max, base, size);
    mpz_pow_ui(min, base, size - 1);
    mpz_sub(diff, max, min);
    
    for (int i = 0; i < N; i++) {
        mpz_urandomm(f1, state, diff);
        mpz_urandomm(f2, state, diff);
        mpz_init(factors1[i]);
        mpz_init(factors2[i]);
        mpz_add(factors1[i], f1, min);
        mpz_add(factors2[i], f2, min);
    }
}

/****************
 * MAIN ROUTINE *
 ****************/

int main(int argc, char *argv[]) {
    /* === Initialize variables for testing  === */

    int N;
    if (argc != 2) { // Only one argument
        printf("Usage: ./main <N>\n");
        printf("N is number of iterations (multiplications per bitwidth + size combination\n");
        return -1;
    }
    if (!str_is_int(argv[1]) || ((N = atoi(argv[1])) <= 0)) { // Must be positive integer
        printf("N must be a positive integer");
        return -1;
    }
    int k; // size power (of 2)
    long bitwidth, size; // number of bits per point in FFT, number of points
    double t0, t_total_gmp, t_total_fft; // timing


    /* === FFT variables === */
    
    // output of int_mult
    char *result;

    // intermediate variables
    dd_I *outA, *outB, *twiddle;
    long *b4;
    
    // Set rounding mode
    fesetround(FE_UPWARD);


    /* === Initialize GMP variables  === */

    // array of N multiplicands, array of N multipliers, the product calculated
    // by GMP, the product calculated by FFT mult
    mpz_t *factors1, *factors2, gmp_product, fft_product;
    mpz_init(gmp_product);
    mpz_init(fft_product);
    long num_bits;

    // Random state
    gmp_randstate_t state;
    gmp_randinit_default(state);
    unsigned long seed = time(NULL);
    gmp_randseed_ui(state, seed);


    /* === Redirect print stream to file === */
    freopen("output.txt", "a+", stdout);


    /* === Testing loop === */
    printf("=================\n");
    printf("Beginning testing\n");
    printf("=================\n");
    printf("N = %d\n\n", N);

    for (k = 20; k < 30; k++) {
        size = 1 << k; // size = 2^k
        if ((outA = malloc(sizeof(dd_I) * 4 * size)) == NULL) {
            printf("Memory allocation failed");
            return -1;
        }

        if ((outB = malloc(sizeof(dd_I) * 4 * size)) == NULL) {
            printf("Memory allocation failed");
            return -1;
        }

        if ((twiddle = malloc(sizeof(dd_I) * 4 * size)) == NULL) {
            printf("Memory allocation failed");
            return -1;
        }

        if ((b4 = malloc(sizeof(long) * 2 * size)) == NULL) {
            printf("Memory allocation failed");
            return -1;
        }
        result = calloc(sizeof(char), 2 * size);

        printf("Multiplying factors of 2^%d (%ld) digits...\n", k, size);
        for (bitwidth = 1; bitwidth <= 4; bitwidth *= 2) {
            num_bits = size * bitwidth;
            printf("\tbitwidth = %d\n", bitwidth);
            printf("\ttotal bits = %d\n", num_bits);
            t_total_gmp = 0;
            t_total_fft = 0;

            if ((factors1 = malloc(sizeof(mpz_t) * N)) == NULL) {
                printf("Memory allocation failed");
                return -1;
            }

            if ((factors2 = malloc(sizeof(mpz_t) * N)) == NULL) {
                printf("Memory allocation failed");
                return -1;
            }
            
            printf("\t\tGenerating random numbers... ");
            fflush(stdout);
            t0 = omp_get_wtime();
            random_factors(factors1, factors2, size, bitwidth, state, N);
            printf("Finished in %lf s\n", omp_get_wtime() - t0);

            printf("\t\tGenerating twiddles... ");
            fflush(stdout);
            t0 = omp_get_wtime();
            fulldd_initialize_twiddles(2 * size, twiddle);
            printf("Finished in %lf s\n", omp_get_wtime() - t0);

            char *factor1 = malloc(sizeof(char) * size);
            char *factor2 = malloc(sizeof(char) * size);

            printf("\t\tRunning %d multiplications... ", N);
            fflush(stdout);
            for (int i = 0; i < N; i++) {
                mpz_get_str(factor1, 1 << bitwidth, factors1[i]);
                mpz_get_str(factor2, 1 << bitwidth, factors2[i]);
                // printf("\n%s %s\n", factor1, factor2);
                fulldd_readinit(outA, bitwidth, size, factor1);
                fulldd_readinit(outB, bitwidth, size, factor2);

                t0 = omp_get_wtime();
                fulldd_int_mult(bitwidth, size, outA, outB, twiddle);
                t_total_fft += omp_get_wtime() - t0;
                fulldd_turn_it_back(bitwidth, 2*size, outA, b4);
                perform_carry(bitwidth, b4, 2*size);
                write_result(b4, result, 2*size, bitwidth);
                // gmp_printf("\n%Zd, %Zd\n", factors1[i], factors2[i]);
                // printf("\nFFT_RESULT = %s\n", result);
                mpz_set_str(fft_product, result, 1 << bitwidth);
                // gmp_printf("\nFFT_RESULT = %Zd\n", fft_product);

                t0 = omp_get_wtime();
                mpz_mul(gmp_product, factors1[i], factors2[i]);
                // gmp_printf("\nGMP_RESULT = %Zd\n", gmp_product);
                t_total_gmp += omp_get_wtime() - t0;

                if (mpz_cmp(fft_product, gmp_product) != 0) {
                    printf("FAILURE: Incorrect result at size = 2^%d\n", k);
                    continue;
                }
            }
            printf("Finished!\n");

            printf("\t\tavg_time_gmp = %lf ms\n", t_total_gmp / (double)N * 1000);
            printf("\t\tavg_time_fft = %lf ms\n", t_total_fft / (double)N * 1000);
        }

        free(outA);
        free(outB);
        free(b4);
        free(twiddle);
        free(result);
    }
}
