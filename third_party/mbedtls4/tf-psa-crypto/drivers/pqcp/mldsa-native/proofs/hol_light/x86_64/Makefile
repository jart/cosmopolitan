#############################################################################
# Copyright (c) The mldsa-native project authors
# Copyright Amazon.com, Inc. or its affiliates. All Rights Reserved.
# SPDX-License-Identifier: Apache-2.0 OR ISC OR MIT-0
#############################################################################

#
# This Makefile is derived from the Makefile x86/Makefile in s2n-bignum.
# - Remove all s2n-bignum proofs and tutorial, add mldsa-native proofs
# - Minor path modifications to support base theories from s2n-bignum
#   to reside in a separate read-only directory
#

.DEFAULT_GOAL := run_proofs

OSTYPE_RESULT=$(shell uname -s)
ARCHTYPE_RESULT=$(shell uname -m)

SRC ?= $(S2N_BIGNUM_DIR)
SRC_X86 ?= $(SRC)/x86

# If actually on an x86_64 machine, just use the assembler (as). Otherwise
# use a cross-assembling version so that the code can still be assembled
# and the proofs checked against the object files (though you won't be able
# to run code without additional emulation infrastructure).
ifeq ($(filter $(ARCHTYPE_RESULT),x86_64),)
CROSS_PREFIX=x86_64-unknown-linux-gnu-
ifeq ($(shell command -v $(ASSEMBLE) >/dev/null 2>&1 && echo yes || echo no),no)
$(error Cross-toolchain not found. Please run in the 'hol_light-cross-x86_64' nix shell via: nix develop .#hol_light-cross-x86_64)
endif
endif
ASSEMBLE=$(CROSS_PREFIX)as $(ARCHFLAGS)
OBJDUMP=$(CROSS_PREFIX)objdump -d

# Add explicit language input parameter to cpp, otherwise the use of #n for
# numeric literals in x86 code is a problem when used inside #define macros
# since normally that means stringization.
#
# Some clang-based preprocessors seem to behave differently, and get confused
# by single-quote characters in comments, so we eliminate // comments first.

ifeq ($(OSTYPE_RESULT),Darwin)
PREPROCESS=sed -e 's/\/\/.*//' | $(CC) -E -xassembler-with-cpp -
else
PREPROCESS=$(CC) -E -xassembler-with-cpp -
endif

# Generally GNU-type assemblers are happy with multiple instructions on
# a line, but we split them up anyway just in case.

SPLIT=tr ';' '\n'

OBJ = mldsa/mldsa_ntt.o

# Build object files from assembly sources
$(OBJ): %.o : %.S
	@echo "Preparing $@ ..."
	@echo "AS: `$(ASSEMBLE) --version`"
	@echo "OBJDUMP: `$(OBJDUMP) --version`"
	$(Q)[ -d $(@D) ] || mkdir -p $(@D)
	cat $< | $(PREPROCESS) | $(SPLIT) | $(ASSEMBLE) -o $@ -
	# MacOS may generate relocations in non-text sections that break
	# the object file parser in HOL-Light
	strip $@

clean:; rm -f */*.o */*/*.o */*.correct */*.native

# Proof-related parts
#
# The proof files are all independent, though each one loads the
# same common infrastructure "base.ml". So you can potentially
# run the proofs in parallel for more speed, e.g.
#
#    nohup make -j 16 proofs &
#
# If you build hol-light yourself (see https://github.com/jrh13/hol-light)
# in your home directory, and do "make" inside the subdirectory hol-light,
# then the following HOLDIR setting should be right:

HOLDIR?=$(HOLLIGHTDIR)
HOLLIGHT:=$(HOLDIR)/hol.sh

BASE?=$(shell dirname $(realpath $(firstword $(MAKEFILE_LIST))))

PROOF_BINS = $(OBJ:.o=.native)
PROOF_LOGS = $(OBJ:.o=.correct)

# Build precompiled binary for dumping bytecodes
proofs/dump_bytecode.native: proofs/dump_bytecode.ml $(OBJ)
	./proofs/build-proof.sh $(BASE)/$< "$(HOLLIGHT)" "$@"

# Build precompiled native binaries of HOL Light proofs

.SECONDEXPANSION:
%.native: proofs/$$(*F).ml %.o ; ./proofs/build-proof.sh $(BASE)/$< "$(HOLLIGHT)" "$@"

# Run them and print the standard output+error at *.correct
%.correct: %.native
	$< 2>&1 | tee $@
	@if (grep -i "error:\|exception:" "$@" >/dev/null); then  \
		echo "$< had errors!";			          \
		exit 1;					          \
	else							  \
		echo "$< OK";					  \
	fi

build_proofs: $(PROOF_BINS);
run_proofs: build_proofs $(PROOF_LOGS);

proofs: run_proofs ; $(SRC)/tools/count-proofs.sh .

dump_bytecode: proofs/dump_bytecode.native
	./$<

.PHONY: proofs build_proofs run_proofs sematest clean dump_bytecode

# Always run sematest regardless of dependency check
FORCE: ;
# Always use max. # of cores because in Makefile one cannot get the passed number of -j.
# A portable way of getting the number of max. cores:
# https://stackoverflow.com/a/23569003/1488216
NUM_CORES_FOR_SEMATEST = $(shell getconf _NPROCESSORS_ONLN)
sematest: FORCE $(OBJ) $(SRC_X86)/proofs/simulator_iclasses.ml $(SRC_X86)/proofs/simulator.native
	$(SRC)/tools/run-sematest.sh x86 $(NUM_CORES_FOR_SEMATEST)
