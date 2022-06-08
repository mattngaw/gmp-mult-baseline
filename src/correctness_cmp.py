import decimal, gmpy2, random, sys, subprocess, time

FILE_X = "temp/factor1.txt"
FILE_Y = "temp/factor2.txt"

GMP_BASE = "./gmp_baseline"

PROD_GMP = "temp/product-gmp.txt"
PROD_TEST = "temp/product-test.txt"

def main():
    print("Starting correctness comparison...")

    # Check arguments
    if len(sys.argv) != 4:
        print(f"Invalid number of arguments ({len(sys.argv)-1}). Need 3")
        return

    # Arguments
    exe = sys.argv[1] # executable that multiplies
    n = int(sys.argv[2]) # number of tests to run
    k = int(sys.argv[3]) # bit length of the factors

    print(f"Generating {n} factor pairs of 2^{k} bits... ", end="", flush=True)
    t0 = time.time()
    factors = random_factors(n, 2**int(k)) # generate random factors
    print(f"Finished in {round(time.time() - t0, 10)}s")

    print("Opening factor 1... ", end="", flush=True)
    f1 = open(FILE_X, mode='w', encoding="utf-8")
    print("Finished!")

    print("Opening factor 2... ", end="", flush=True)
    f2 = open(FILE_Y, mode='w', encoding="utf-8")
    print("Finished!")

    prod_gmp_f = None
    prod_test_f = None

    # input("Enter to begin:")

    for i in range(n):
        x, y = factors[i]
        print("\t Writing factor 1... ", end="", flush=True)
        t0 = time.time()
        f1.write(x)
        print(f"Finished in {round(time.time() - t0, 10)}s")
        print("\t Writing factor 2... ", end="", flush=True)
        t0 = time.time()
        f2.write(y)
        print(f"Finished in {round(time.time() - t0, 10)}s")

        p1_args = [GMP_BASE, FILE_X, str(len(x)), FILE_Y, str(len(y)), PROD_GMP]
        p2_args = [exe, FILE_X, str(len(x)), FILE_Y, str(len(y)), PROD_TEST, k]
        print("\t Running GMP mult... ", end="")
        t0 = time.time()
        p1 = subprocess.run(p1_args)
        print(f"Finished in {round(time.time() - t0, 10)}s")
        print("\t Running test mult... ", end="", flush=True)
        t0 = time.time()
        p2 = subprocess.run(p2_args)
        print(f"Finished in {round(time.time() - t0, 10)}s")

        p1.check_returncode()
        p2.check_returncode()

        if not prod_gmp_f: prod_gmp_f = open(PROD_GMP, 'r')
        if not prod_test_f: prod_test_f = open(PROD_TEST, 'r')

        print("\t Reading GMP product... ", end="", flush=True)
        t0 = time.time()
        prod_gmp = prod_gmp_f.read()
        print(f"Finished in {round(time.time() - t0, 10)}s")
        print("\t Reading test product... ", end="", flush=True)
        t0 = time.time()
        prod_test = prod_test_f.read()
        print(f"Finished in {round(time.time() - t0, 10)}s")
        print("Finished!")
        print("Comparing results... ", end="", flush=True)
        t0 = time.time()
        if prod_gmp == prod_test:
            print("Correct ", end="", flush=True)
        else:
            print("Incorrect ", end="", flush=True)
        print(f"in {round(time.time() - t0, 10)}s")

    f1.close()
    f2.close()
    prod_gmp_f.close()
    prod_test_f.close()

    return


def random_factors(n, k) -> list:
    random_state = gmpy2.random_state()
    factors = []
    for _ in range(n):
        x = gmpy2.mpz_urandomb(random_state, k)
        y = gmpy2.mpz_urandomb(random_state, k)
        x = x.digits()
        y = y.digits()
        factors.append((x, y))
    # factors = [(gmpy2.mpz_urandomb(random_state, k).digits(),
    #             gmpy2.mpz_urandomb(random_state, k).digits())
    #            for _ in range(n)]
    # factors = [(str(gmpy2.mpz_urandomb(random_state, k)),
    #             str(gmpy2.mpz_urandomb(random_state, k)))
    #            for _ in range(n)]
    return factors

if __name__ == "__main__":
    main()
