(*
 * Copyright Amazon.com, Inc. or its affiliates. All Rights Reserved.
 * SPDX-License-Identifier: Apache-2.0 OR ISC OR MIT-0
 *)

needs "x86/proofs/base.ml";;

print_string "=== bytecode start: mldsa/mldsa_ntt.o ================\n";;
print_literal_from_elf "mldsa/mldsa_ntt.o";;
print_string "==== bytecode end =====================================\n\n";;
