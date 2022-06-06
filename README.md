# GMP Multiplication Baseline

A Python script used to test against [GMP's](https://gmplib.org/) implementation of integer multiplication (see [GMP's documentation](https://gmplib.org/manual/)).

## Setup

This script requires Python 3.

Clone the repository.

```sh
git clone https://github.com/mattngaw/gmp-mult-baseline.git

cd ./gmp-mult-baseline
```

Download the [latest version of GMP](https://gmplib.org/) (download the `.tar.lz`).

Place it in the `lib` directory and extract.

```sh
tar --lzip -xvf ~/some/path/to/gmp-#.#.#.tar.lz
```

Build GMP for your system.
```sh
cd ./lib/gmp-#.#.#

./configure

make
```

For extra help see: https://gmplib.org/manual/Introduction-to-GMP

## Usage

The script takes three arguments:

`path`: The path to an executable that you want to test against GMP

> This executable must take in 5 arguments
> `f1`: Path to text file containing factor 1
> `len1`: Number of bytes (i.e. characters/digits) in `f1`
> `f2`: Path to text file containing factor 2
> `len2`: Number of bytes in `f2`
> `f3`: Path to the output file into which the product will be written
> ```
> ./your_executable <f1> <len1> <f2> <len2> <f3>
> ```

`n`: Total number of tests to run (i.e. how many multiplications to do)

`k`: Factor power (i.e. bit width of each factor = 2^k)

```sh
python3 src/correctness_cmp.py <path> <n> <k>
```

