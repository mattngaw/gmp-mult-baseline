import decimal, gmpy2, random, sys, subprocess

FILE_X = "temp/factor1.txt"
FILE_Y = "temp/factor2.txt"

GMP_BASE = "./gmp_baseline"

PROD_GMP = "temp/product-gmp.txt"
PROD_TEST = "temp/product-test.txt"

def main():
    print("Starting correctness comparison...")

    if len(sys.argv) != 4:
        print(f"Invalid number of arguments ({len(sys.argv)-1}). Need 3")
        return

    exe = sys.argv[1]
    n = int(sys.argv[2])
    k = int(sys.argv[3])

    print("Generating factors... ", end="")
    factors = random_factors(n, 2**int(k))
    print("Finished!")

    print("Opening factor 1... ", end="")
    f1 = open(FILE_X, mode='w', encoding="utf-8")
    print("Finished!")

    print("Opening factor 2... ", end="")
    f2 = open(FILE_Y, mode='w', encoding="utf-8")
    print("Finished!")

    prod_gmp_f = None
    prod_test_f = None

    # input("Enter to begin:")

    for i in range(n):
        x, y = factors[i]
        x, y = str(x), str(y)
        print("\t Writing factor 1... ", end="")
        f1.write(x)
        print("Finished!")
        print("\t Writing factor 2... ", end="")
        f2.write(y)
        print("Finished!")

        p1_args = [GMP_BASE, FILE_X, str(len(x)), FILE_Y, str(len(y)), PROD_GMP]
        p2_args = [exe, FILE_X, str(len(x)), FILE_Y, str(len(y)), PROD_TEST]
        print("\t Running GMP mult... ", end="")
        p1 = subprocess.run(p1_args)
        print("Finished!")
        print("\t Running test mult... ", end="")
        p2 = subprocess.run(p2_args)
        print("Finished!")

        p1.check_returncode()
        p2.check_returncode()

        if not prod_gmp_f: prod_gmp_f = open(PROD_GMP, 'r')
        if not prod_test_f: prod_test_f = open(PROD_TEST, 'r')

        print("\t Reading GMP product... ", end="")
        prod_gmp = prod_gmp_f.read()
        print("Finished!")
        print("\t Reading test product... ", end="")
        prod_test = prod_test_f.read()
        print("Finished!")
        print("Comparing results... ", end="")
        if prod_gmp == prod_test:
            print("Correct!")
        else:
            print("Incorrect!")

    f1.close()
    f2.close()
    prod_gmp_f.close()
    prod_test_f.close()

    return


def random_factors(n, k) -> list:
    random_state = gmpy2.random_state()
    factors = [(gmpy2.mpz_urandomb(random_state, k),
                gmpy2.mpz_urandomb(random_state, k))
               for _ in range(n)]
    return factors

if __name__ == "__main__":
    main()
