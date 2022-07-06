# GMP Multiplication Baseline

A C routine used to test an FFT-based multiplication algorithm against [GMP's](https://gmplib.org/) implementation of integer multiplication (see [GMP's documentation](https://gmplib.org/manual/)).

## Usage

The script takes one argument:

`N`: Total number of tests to run (i.e. how many multiplications to do)

```sh
./main <N>
```

