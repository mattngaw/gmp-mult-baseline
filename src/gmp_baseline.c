#include <time.h>
#include <stdlib.h>
#include <stdio.h>
#include <gmp.h>

int main(int argc, char *argv[]) {
    mpz_t x, y, z; // Factors (x, y) and result (z)
    char *x_file_name, *y_file_name, *z_file_name; // Paths to the factor files
    FILE *x_file, *y_file, *z_file; // Files containing factors for multiplication
    int x_str_len, y_str_len; // Number of characters in each factor file
    clock_t t;
    int ms;

    // Init x, y, z
    mpz_init(x);
    mpz_init(y);
    mpz_init(z);

    // Input error if incorrect number of arguments
    if (argc < 6) {
        fprintf(stderr, "Invalid number of arguments: %d", argc-1);
        return -1;
    }

    // Extract files and lengths from arguments
    x_file_name = argv[1];
    x_str_len = atoi(argv[2]);
    y_file_name = argv[3];
    y_str_len = atoi(argv[4]);
    z_file_name = argv[5];

    // Open x and y files
    if ((x_file = fopen(x_file_name, "r")) == NULL) {
        perror("Error opening x_file");
        return -1;
    }

    if ((y_file = fopen(y_file_name, "r")) == NULL) {
        perror("Error opening y_file");
        return -1;
    }
    z_file = fopen(z_file_name, "w");

    // Allocate space for x and y strings
    char *x_buf = malloc(x_str_len * sizeof(char));
    if (x_buf == NULL) {
        perror("x_buf malloc error");
        return -1;
    }
    char *y_buf = malloc(y_str_len * sizeof(char));
    if (y_buf == NULL) {
        perror("y_buf malloc error");
        return -1;
    }

    // Reading file contents into buffers
    printf("\n\t\t Reading x into buffer...");
    t = clock();
    fgets(x_buf, x_str_len, x_file);
    if (x_buf == NULL) {
        perror("Error writing to x_buf");
        return -1;
    }
    t = clock() - t;
    ms = t * 1000 / CLOCKS_PER_SEC;
    printf("Finished in %dms\n", ms);
    printf("\t\t Reading y into buffer...");
    t = clock();
    fgets(y_buf, y_str_len, y_file);
    if (y_buf == NULL) {
        perror("Error writing to y_buf");
        return -1;
    }
    t = clock() - t;
    ms = t * 1000 / CLOCKS_PER_SEC;
    printf("Finished in %dms\n", ms);

    // Set x and y from strings
    mpz_set_str(x, x_buf, 10);
    mpz_set_str(y, y_buf, 10);

    // Multiply
    printf("\t\t GMP multiplying...");
    t = clock();
    mpz_mul(z, y, x);
    t = clock() - t;
    ms = t * 1000 / CLOCKS_PER_SEC;
    printf("Finished in %dms\n", ms);
    printf("\t Writing product into file...");
    t = clock();
    gmp_fprintf(z_file, "%Zd", z);
    t = clock() - t;
    ms = t * 1000 / CLOCKS_PER_SEC;
    printf("Finished in %dms\n", ms);

    fclose(x_file);
    fclose(y_file);
    fclose(z_file);

    free(x_buf);
    free(y_buf);

    mpz_clear(x);
    mpz_clear(y);
    mpz_clear(z);

    return 0;
}
