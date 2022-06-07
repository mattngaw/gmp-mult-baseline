#include <time.h>
#include <stdlib.h>
#include <stdio.h>
#include <gmp.h>

int main(int argc, char *argv[]) {
    mpz_t x, y, z; // Factors (x, y) and result (z)
    char *x_file_name, *y_file_name, *z_file_name; // Paths to the factor files
    FILE *x_file, *y_file, *z_file; // Files containing factors for multiplication
    int x_str_len, y_str_len; // Number of characters in each factor file
    
    // Init x, y, z
    mpz_init(x);
    mpz_init(y);
    mpz_init(z);

    // Input error if incorrect number of arguments
    if (argc != 6) {
        fprintf(stderr, "Invalid number of arguments: %d", argc-1);
        return 1;
    }

    // Extract files and lengths from arguments
    x_file_name = argv[1];
    x_str_len = atoi(argv[2])+1;
    y_file_name = argv[3];
    y_str_len = atoi(argv[4])+1;
    z_file_name = argv[5];

    // Open x and y files
    x_file = fopen(x_file_name, "r");
    y_file = fopen(y_file_name, "r");
    z_file = fopen(z_file_name, "w");
    
    // Allocate space for x and y strings
    char *x_buf = malloc(x_str_len * sizeof(char));
    char *y_buf = malloc(y_str_len * sizeof(char));

    // Write into buffers
    fgets(x_buf, x_str_len, x_file);
    fgets(y_buf, y_str_len, y_file);

    // Set x and y from strings
    mpz_set_str(x, x_buf, 10);
    mpz_set_str(y, y_buf, 10);

    // Multiply
    mpz_mul(z, y, x);
    gmp_fprintf(z_file, "%Zd", z);

    fclose(x_file);
    fclose(y_file);
    fclose(z_file);

    return 0;
}
