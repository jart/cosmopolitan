[//]: # (SPDX-License-Identifier: Apache-2.0 OR ISC OR MIT-0)

# HOL Light functional correctness proofs

This directory contains functional correctness proofs for some of the x86_64 assembly routines
used in mldsa-native. The proofs were largely developed by John Harrison and Jake Massimo,
and are written in the [HOL Light](https://hol-light.github.io/) theorem
prover, utilizing the assembly verification infrastructure from [s2n-bignum](https://github.com/awslabs/s2n-bignum).

Each function is proved in a separate `.ml` file in [proofs/](proofs). Each file
contains the byte code being verified, as well as the specification that is being
proved.

## Primer

Proofs are 'post-hoc' in the sense that HOL-Light/s2n-bignum operate on the final object code. In particular, the means by which the code was generated need not be trusted.

Specifications are essentially [Hoare triples](https://en.wikipedia.org/wiki/Hoare_logic), with the noteworthy difference that the program is implicit as the content of memory at the PC; which is asserted to
be the code under verification as part of the precondition. For example, the following is the specification of the `mldsa_ntt` function:

```ocaml
 (* For all (abbreviated by `!` in HOL):
    - a: Polynomial coefficients pointer
    - zetas: NTT constants pointer
    - x: Original polynomial coefficients
    - pc: Current value of Program Counter (PC)
    - stackpointer: Stack pointer
    - returnaddress: Return address on the stack *)
`!a zetas x pc stackpointer returnaddress.
    (* Alignment and non-overlapping requirements *)
    aligned 32 a /\
    aligned 32 zetas /\
    nonoverlapping (word pc,LENGTH mldsa_ntt_mc) (a, 1024) /\
    nonoverlapping (word pc,LENGTH mldsa_ntt_mc) (zetas, 2496) /\
    nonoverlapping (a, 1024) (zetas, 2496) /\
    nonoverlapping (stackpointer,8) (a, 1024) /\
    nonoverlapping (stackpointer,8) (zetas, 2496)
    ==> ensures x86
      (* Precondition *)
      (\s. (* The memory at the current PC is the byte-code of mldsa_ntt() *)
        bytes_loaded s (word pc) mldsa_ntt_mc /\
        read RIP s = word pc /\
        read RSP s = stackpointer /\
        (* The return address is on the stack *)
        read (memory :> bytes64 stackpointer) s = returnaddress /\
        (* Arguments are passed via C calling convention *)
        C_ARGUMENTS [a; zetas] s /\
        (* NTT constants are properly loaded *)
        wordlist_from_memory(zetas,624) s = MAP (iword: int -> 32 word) mldsa_complete_qdata /\
        (* Input bounds checking *)
        (!i. i < 256 ==> abs(ival(x i)) <= &8380416) /\
        (* Give a name to the memory contents at the source pointer *)
        !i. i < 256
            ==> read(memory :> bytes32(word_add a (word(4 * i)))) s = x i)
      (* Postcondition: Eventually we reach a state where ... *)
      (\s.
        (* The PC is the return address *)
        read RIP s = returnaddress /\
        (* Stack pointer is adjusted *)
        read RSP s = word_add stackpointer (word 8) /\
        (* The integers represented by the final memory contents
         * are congruent to the ML-DSA forward NTT transformation
         * of the original coefficients, modulo 8380417, with proper bounds *)
        !i. i < 256
            ==> let zi = read(memory :> bytes32(word_add a (word(4 * i)))) s in
                (ival zi == mldsa_forward_ntt (ival o x) i) (mod &8380417) /\
                abs(ival zi) <= &42035261)
      (* Footprint: The program may modify (only) the ABI permitted registers
       * and flags, stack pointer, and the memory contents at the source pointer. *)
      (MAYCHANGE [RSP] ,, MAYCHANGE_REGS_AND_FLAGS_PERMITTED_BY_ABI ,,
       MAYCHANGE [memory :> bytes(a,1024)])`
```

## Platform Compatibility

These x86_64 assembly proofs are expected to work on the following platforms:

- **Linux x86_64**: Fully supported (native build and execution)
- **Linux AArch64**: Not currently supported. Requires x86_64 cross-compilation tools (`x86_64-linux-gnu-as` and `x86_64-linux-gnu-objdump`).
- **macOS x86_64**: Fully supported (native build and execution)
- **macOS ARM (Apple Silicon)**: Not currently supported. Differences in assembler behavior between clang-based (macOS) and gcc-based (Linux) toolchains can cause verification mismatches, particularly with regard to argument ordering optimizations for commutative instructions.

## Reproducing the proofs

To reproduce the proofs, enter the nix shell via

```bash
nix develop --experimental-features 'nix-command flakes'
```

from mldsa-native's base directory. Then

```bash
make -C proofs/hol_light/x86_64
```

will build and run the proofs. Note that this make take hours even on powerful machines.

For convenience, you can also use `tests hol_light` which wraps the `make` invocation above; see `tests hol_light --help`.

## What is covered?

Currently this includes:

- ML-DSA Arithmetic:
  * x86_64 forward NTT: [mldsa_ntt.S](mldsa/mldsa_ntt.S)

The NTT function is optimized using AVX2 instructions and follows the s2n-bignum x86_64 assembly verification patterns.
