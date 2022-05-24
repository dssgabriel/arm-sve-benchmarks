# ARM SVE Benchmarks

This repository gathers some small kernels to benchmark the performance of hand-written SVE code compared to compiler-generated one.
Implemented kernels are:
- Initialization (store);
- Copy (load, store);
- Reduction (load, add);
- Dot product (load, load, mul, add);
- DAXPY (load, load, load, mul, add, store);
- Vector sum (load, load, load, add, store);
- Vector scale (load, load, load, mul, store).

## Usage
*Note:* the provided Makefile uses the `armclang` compiler, however both `clang` and `gcc` have been tested and can be used as well.
Keep in mind that the architecture specific flags (`AFLAGS`) might need to be changed depending on the chosen compiler.
See the [comparison between compiler flags across architectures](https://community.arm.com/arm-community-blogs/b/tools-software-ides-blog/posts/compiler-flags-across-architectures-march-mtune-and-mcpu) for more information.

To build the benchmarks:
```
make build
```

You can then execute one of the benchmarks presented above and specify the vectors' size (in bytes), number of iterations and error tolerance through the provided option flags.

Example (reduction benchmark with 64KiB vectors, 100k iterations and an error tolerance of $10^{-14}$:
```
target/arm_bench -k reduc -s 8192 -i 100000 -e 1e-14
```
